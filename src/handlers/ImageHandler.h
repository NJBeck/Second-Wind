#pragma once
// to manage the storage and access of image files

// TODO: actually do garbage collection for data
// actually pack the data into data instead of pointers may improve performance

#include <unordered_map>
#include <map>
#include <string>
#include <array>
#include <unordered_set>

#include "globals.h"

class ImageHandler {
public:
	enum class Image {
		WALK_BODY_MALE
	};
	// POD for data about image
	class ImageMetaData {
	public:
		int width = 0;	// width of image in pixels
		int height = 0;
		int channels = 3;	// if there is alpha channel
		int rows = 1;
		int columns = 1;
	};

	void Add(EntityID const handle, Image);

	// tells handler that this entity no longer uses this image
	void Remove(EntityID const handle, Image);
	// returns meta data for image
	ImageMetaData GetImageData(Image);
	unsigned char* GetImagePtr(Image);
	// returns the data for all the images registered to the given entity
	std::vector<ImageMetaData> GetData(EntityID const handle) const;
private:
	// maps the file to what entities reference it
	// used to manage lifetime of file in memory 
	// (deleted with no longer referenced)
	// and makes sure theres only 1 of each image
	struct ImageData{
		ImageMetaData meta_data;
		int count = 0;
        std::string path = "";
        unsigned char* data_ptr = nullptr;
		ImageData(int row, int cols, int chans, std::string pat){ 
			meta_data.rows = row;
			meta_data.columns = cols;
			meta_data.channels = chans;
			path = pat;
		};
        ImageData() = default;
	};
	std::unordered_map<Image, ImageData> registry_{
            {Image::WALK_BODY_MALE,
             ImageData{4, 9, 3, "walkcycle/BODY_male.png"}}
	};

	// maps a handle to the data for the images it uses
	std::unordered_map<EntityID, std::unordered_set<Image>> index_;

};