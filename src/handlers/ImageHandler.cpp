#include "ImageHandler.h"
#include "utility.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <stdexcept>


using std::vector, std::string, std::runtime_error, std::to_string, std::array;

void ImageHandler::Add(EntityID const handle, vector<Image> const& imgs) {
	for (auto& img : imgs) {
		// load image if reference count is 0 then increments
		auto& img_data = image_data[static_cast<u32>(img)];
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
			img_data.width = width;
			img_data.height = height;
			img_data.channels = channels;
			img_data.image_ptr = imagePtr;
		}
		img_data.count++;

		// generate and configure texture 
		// -------------------------
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glCheckError();
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glCheckError();
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glCheckError();
		// load textures and generate mipmaps
		if (img_data.channels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_data.width, img_data.height,
				0, GL_RGB, GL_UNSIGNED_BYTE, img_data.image_ptr);
			glCheckError();
			glGenerateMipmap(GL_TEXTURE_2D);
			glCheckError();
		}
		else {

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_data.width, img_data.height,
				0, GL_RGBA, GL_UNSIGNED_BYTE, img_data.image_ptr);
			glCheckError();
			glGenerateMipmap(GL_TEXTURE_2D);
			glCheckError();
		}
	}
}

void ImageHandler::Remove(EntityID const handle, vector<Image> const& imgs) {
	for (auto& img : imgs) {
		auto& img_data = image_data[static_cast<u32>(img)];
		if (img_data.count == 0) {
			string exep = "cannot deregister entity " + to_string(handle) +
						  " from " + img_data.path;
			throw runtime_error(exep);
		}
		img_data.count--;
		index_[handle].erase(img);
	}
}

ImageHandler::ImageData const& ImageHandler::GetImageData(Image const img) const
{
	return image_data[static_cast<u32>(img)];
}

std::vector<ImageHandler::ImageData> const 
ImageHandler::GetEntityData(EntityID const handle) const
{
	vector<ImageHandler::ImageData> return_vec;
	auto found = index_.find(handle);
	if (found == index_.end()) {
		// error stuff
	}
	auto& img_set = found->second;
	for (auto& img : img_set) {
		return_vec.emplace_back(image_data[static_cast<u32>(img)]);
	}
	return return_vec;
}



auto ImageHandler::image_data = array{
	ImageData(Image::WALK_BODY_MALE, 4, 9, 3, "walkcycle/BODY_male.png")
};
