#include "ImageHandler.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <stdexcept>


using namespace std;

void ImageHandler::Add(uint64_t handle, string filePath) {
	auto search = registry_.find(filePath);
	if (search == registry_.end()) {
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		unsigned char* imagePtr = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
		if (!imagePtr)
		{
			string err = "could not load image " + filePath;
			throw runtime_error(err);
		}
		registry_[filePath] = { 1, imagePtr };
		index_[handle].emplace(imagePtr, width, height, channels);
	}
	else {
		search->second.count++;
		auto data = search->second.data;
		index_[handle].emplace(data.dataPtr, data.width, 
							   data.height, data.channels);
	}
}

void ImageHandler::Remove(uint64_t handle, string filePath) {
	auto found = registry_.find(filePath);
	if (found != registry_.end()) {
		auto entity_file_iter = index_[handle].begin();
		while (entity_file_iter->first != found->second.data.dataPtr) {
			++entity_file_iter;
		}
		if (entity_file_iter == index_[handle].end()) {
			string err = "tried to remove file " + filePath + 
						 " from non-possessing entity";
			throw runtime_error(err);
		}
		index_[handle].erase(entity_file_iter);
	}
}

vector<ImageHandler::ImageData> 
ImageHandler::GetData(uint64_t const handle) const
{	
	vector<ImageData> result;
	auto found = index_.find(handle);
	if (found != index_.end()) {
		for (auto& imgData : found->second) {
			result.emplace_back(imgData.first, imgData.second[0],
								imgData.second[1], imgData.second[2]);
		}
	}
	return result;
}

ImageHandler::ImageData ImageHandler::GetImgData(std::string const src) const
{
	auto found = registry_.find(src);
	if (found != registry_.end()) {
		return found->second.data;
	}
	else {
		string err = src + " not found in image registry";
		throw runtime_error(err);
	}
}
