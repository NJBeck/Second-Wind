#pragma once
// to manage the storage and access of image files

// TODO: actually do garbage collection for data
// actually pack the data into data instead of pointers may improve performance

#include <set>
#include <unordered_map>
#include <string>
#include <array>

struct ImageData {
	unsigned char* dataPtr;
	int width;	// width of image in pixels
	int height;
	int channels;	// if there is alpha channel
};

class ImageHandler {

	// maps the file to what entities reference it
	// used to manage lifetime of file in memory (deleted with no longer referenced)
	// and makes sure theres only 1 of each image
	std::unordered_map<std::string, std::set<unsigned long>> registry;

	// the actual array of data for all images loaded
	std::vector<ImageData> data;

	// maps a handle to the index in data of the entity's data
	std::unordered_map<unsigned long, unsigned int> dataIndex;

public:
	// tells the handler that the entity with this handle uses this image
	// image filepath should be relative to /data/ folder
	void add(unsigned long, std::string);

	// tells handler that this entity no longer uses this image
	void del(unsigned long, std::string);

	ImageData GetData(unsigned long handle) { return data[dataIndex[handle]]; }

};