#include "ImageHandler.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <iostream>

using std::vector, std::string, std::cout, std::endl;

void ImageHandler::add(uint64_t handle, string filePath) {
	auto search = registry.find(filePath);
	if (search == registry.end()) {
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		unsigned char* imageData = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
		if (!imageData)
		{
			std::cout << "Failed to load image:" << filePath << std::endl;
		}
		data.push_back({imageData, width, height, channels});
		dataIndex[handle] = data.size() - 1;
	}
	registry[filePath].emplace(handle);
}

void ImageHandler::del(uint64_t handle, string filePath) {
	std::set<uint64_t>& handleSet = registry[filePath];
	auto search = handleSet.find(handle);
	if (search != handleSet.end()) {
		// if the handle has a reference to this image we remove it
		handleSet.erase(search);
		if (handleSet.empty()) {
			// if that was the last one we delete the image
			ImageData& im = data[dataIndex[handle]];
			delete[] im.dataPtr;
			// we set the dataPtr of the ImageData to NULL for future garbage collector use
			im.dataPtr = NULL;
		}
	}
	else {
		cout << "entity: " << handle << "tried to del from ImageHandler but wasn't found" << endl;
	}
}
