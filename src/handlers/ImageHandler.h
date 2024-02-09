#pragma once
// to manage the storage and access of image files

// TODO: actually do garbage collection for data
// actually pack the data into data instead of pointers may improve performance

#include <unordered_map>
#include <map>
#include <string>
#include <array>
#include <unordered_set>
#include <vector>

#include "glad/glad.h"
#include "globals.h"

class ImageHandler {
public:
	enum class Image {
		WALK_BODY_MALE,
		// COUNT stores # of images
		COUNT
	};
	// registers image(s) with entity
	void Add(EntityID const handle, std::vector<Image> const&);
	// tells handler that this entity no longer uses these images
	void Remove(EntityID const handle, std::vector<Image> const&);
	struct ImageData {
		Image image;
		u32 width;
		u32 height;
		u32 channels = 3;	// if there is alpha channel
		u32 rows = 1;
		u32 columns = 1;
		u32 count = 0;
		unsigned char* image_ptr;
		std::string path = "";	// path to image file from data folder
		u32 ID = 0;	// ID for texture
		ImageData(Image img_, int rows_, int cols_, int chans_, std::string path_)
			: image(img_), rows(rows_), columns(cols_), channels(chans_), path(path_) {
		};
		ImageData() = default;
	};
	// returns meta data for image
	ImageData const& GetImageData(Image const) const;
	// returns the data for all the images registered to the given entity
	std::vector<ImageData> const GetEntityData(EntityID const handle) const;
	// data for the images where image's enum is the index for its data
	static std::array<ImageData, static_cast<u32>(Image::COUNT)> image_data;
private:
	// maps a handle to the data for the images it uses
	std::unordered_map<EntityID, std::unordered_set<Image>> index_;
};