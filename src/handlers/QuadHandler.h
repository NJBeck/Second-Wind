#pragma once

#include <string>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <array>

#include "rendering/shader.h"
#include "ImageHandler.h"
#include "globals.h"
#include "utility.h"

class QuadHandler
{
public:

	// the data necessary to draw the quad
	struct GLQuadData {
		GLuint texture;		// texture ID
		GLuint VAO;			// vertex array object for quad
	};
	// Image, rows, column of quad in image
	struct QuadParams {
		ImageHandler::Image image;
		u32 rows;
		u32 columns;
	};
	// sets the active quad for the entity
	void SetActive(EntityID const, QuadParams const&);
	// generates textures and buffers and associates sets of quads with entity
	// first quad is the active quad by default
	void Add(EntityID const, std::vector<QuadParams> const&);
	// remember to add new active if removing active quad from entity
	void Remove(EntityID const, std::vector<QuadParams> const&);
	// returns GLQuadData for that animation state of that quad
	GLQuadData const GetActiveData(EntityID const handle) const;

	QuadHandler(ImageHandler& image_handler);
private:
	//// pairs an entity's active VAO + Image with the set of all of them
	struct ImageVAOs {
		ImageHandler::Image active_image;
		std::unordered_set<ImageHandler::Image> image_set;
		u32 active_VAO;
		std::unordered_set<u32> VAO_set;
	};
	std::unordered_map<EntityID, ImageVAOs> index_;

	GLuint EBO_;	// all quads use the same Element buffer
	ImageHandler& image_handler_;

	// stores the matrices of the VAO VBO and count of entity's referencing it
	// for the row x column of a texture/image
	struct QuadData {
		u32 VAO;
		u32 VBO;
		u32 count;
	};
	static std::array<	std::unique_ptr<MatrixBase<QuadData>>, 
						static_cast<u32>(ImageHandler::Image::COUNT)> quad_mats_;
};
