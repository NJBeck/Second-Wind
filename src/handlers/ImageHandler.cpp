#include "ImageHandler.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <stdexcept>


using namespace std;

void ImageHandler::Add(EntityID handle, Image img) {
	// load image if reference count is 0 then increments
	auto img_data = registry_.find(img)->second;
	registry_[img].count++;
	index_[handle].emplace(img);
	if (img_data.count == 0) {
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		unsigned char* imagePtr = stbi_load(img_data.path.c_str(), &width, 
											&height, &channels, 0);
		if (!imagePtr)
		{
			string err = "could not load image " + img_data.path;
			throw runtime_error(err);
		}
		registry_[img].meta_data.width = width;
        registry_[img].meta_data.height = height;
        registry_[img].meta_data.channels = channels;
        registry_[img].data_ptr = imagePtr;	}
}

void ImageHandler::Remove(EntityID const handle, Image img) {
	auto count = registry_[img].count;
	auto found = index_[handle].find(img);
	if (count == 0 || found == index_[handle].end()) {
		string exep = "cannot deregister entity " + to_string(handle) +
					  " from " + registry_[img].path;
		throw runtime_error(exep);
	}
	registry_[img].count--;
	index_[handle].erase(img);
}

ImageHandler::ImageMetaData ImageHandler::GetImageData(Image img)
{
	return registry_[img].meta_data;
}

unsigned char* ImageHandler::GetImagePtr(Image img)
{
	return registry_[img].data_ptr;
}

vector<ImageHandler::ImageMetaData> 
ImageHandler::GetData(EntityID const handle) const
{	
	vector<ImageMetaData> result;
	auto found = index_.find(handle);
	if (found != index_.end()) {
		auto img_set = found->second;
		for (Image img : img_set) {
			auto data = registry_.find(img)->second.meta_data;
			result.push_back(data);
		}
	}
	else {
		string excep = "cannot find entity " + to_string(handle) +
					   " in image index";
		throw runtime_error(excep);
	}

	return result;
}

