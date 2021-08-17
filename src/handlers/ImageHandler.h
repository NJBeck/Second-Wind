#pragma once
// to manage the storage and access of image files

// TODO: actually do garbage collection for data
// actually pack the data into data instead of pointers may improve performance

#include <unordered_map>
#include <map>
#include <string>
#include <array>

class ImageHandler {
public:
	struct ImageData {
		unsigned char* dataPtr;
		int width;	// width of image in pixels
		int height;
		int channels;	// if there is alpha channel
	};
	// tells the handler that the entity with this handle uses this image
	// image filepath should be relative to /data/ folder
	void Add(uint64_t const handle, std::string file_path);

	// tells handler that this entity no longer uses this image
	void Remove(uint64_t const handle, std::string file_path);

	std::vector<ImageData> GetData(uint64_t const handle) const;

	ImageData GetImgData(std::string const source) const;

private:
	// maps the file to what entities reference it
	// used to manage lifetime of file in memory 
	// (deleted with no longer referenced)
	// and makes sure theres only 1 of each image
	struct ImageCount {
		uint32_t count;
		ImageData data;
	};
	std::map<std::string, ImageCount> registry_;

	// maps a handle to the data for the images it uses
	std::unordered_map<uint64_t, 
					   std::unordered_map<unsigned char*, 
										  std::array<int, 3> > >index_;

};