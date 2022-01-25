#pragma once
// packages some image data, screen space and shaders into 1 object

// TODO:	handle entity deletion/garbage collection (probably use min heap of open indices)

#include <string>
#include <utility>
#include <unordered_map>
#include <tuple>

#include "rendering/shader.h"
#include "ImageHandler.h"
#include "utility.h"



class QuadHandler
{
public:
	// parameters to generate a quad
	struct QuadParams {
		ImageHandler::Image image;
		int row;	// row of quad's texture in image
		int col;
		std::tuple<ImageHandler::Image, int, int> GetTuple() const;
	};

	// the data necessary to draw the quad (other than the image itself)
	struct GLQuadData {
		GLuint texture;		// texture ID
		GLuint VAO;			// vertex array object for quad
	};

	// sets the active quad for the entity
	void SetActive(EntityID const, QuadParams const&);
	// generates textures and buffers and associates sets of quads with entity
	// first quad is the active quad by default
	void Add(EntityID const, std::vector<QuadParams>const &, 
							 uint32_t const activeQuad = 0);
	// returns GLQuadData for that animation state of that quad
	GLQuadData GetActiveData(EntityID const handle) const;

	QuadHandler(ImageHandler*);
private:
	// data to draw a quad
	std::vector<GLQuadData> GLdata_;
	// maps an entity to a vector of indices for each quad it has
	// and the index of the active quad
	std::unordered_map<EntityID, std::pair<uint32_t,
										   std::vector<uint32_t>>> index_;
	// maps a quad to its index in GLdata_ to see if it's already in data		
	using QuadParamTuple = std::tuple<ImageHandler::Image, int, int>;
	std::unordered_map<	QuadParamTuple, uint32_t, 
						utility::TupleHash<QuadParamTuple>> aliases_;
	// stores the GLuint for already generated textures
	std::unordered_map<ImageHandler::Image, GLuint> textures_;
	GLuint EBO;	// all quads use the same EBO
	ImageHandler* img_handler_;
};
