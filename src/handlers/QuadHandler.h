#pragma once
// packages some image data, screen space and shaders into 1 object

// TODO:	handle entity deletion/garbage collection (probably use min heap of open indices)

#include <string>
#include <utility>
#include <unordered_map>

#include "rendering/shader.h"
#include "ImageHandler.h"

// the data necessary to draw the quad (other than the image itself)
struct GLQuadData {
	Shader shaders;		// ID for generated shader program
	GLuint texture;		// texture ID
	GLuint VAO;			// vertex array object for quad
	GLuint VBO;			// vertex buffer object
	GLuint EBO;			// element buffer object
};

// parameters to generate a quad
struct QuadParams {
	std::string image_path; // path for image relative to /data/ folder
	std::string vertsource;	// file path for the vertex shader source
	std::string fragsource;
	float width;	// screenspace width of quad
	float height;
	int row;	// row of quad's texture in image
	int rows;
	int col;
	int cols;
};

namespace std
{
	template<> struct hash<QuadParams>
	{
		std::size_t operator()(QuadParams const& qp) const noexcept
		{
			std::size_t h1 = std::hash<std::string>{}(qp.image_path);
			std::size_t h2 = h1 ^ (std::hash<string>{}(qp.vertsource) << 1);
			std::size_t h3 = h2 ^ (std::hash<float>{}(qp.width) << 1);
			std::size_t h4 = h3 ^ (std::hash<float>{}(qp.height) << 1);
			std::size_t h5 = h4 ^ (std::hash<int>{}(qp.row) << 1);
			std::size_t h6 = h5 ^ (std::hash<int>{}(qp.rows) << 1);
			std::size_t h7 = h6 ^ (std::hash<int>{}(qp.col) << 1);
			std::size_t h8 = h7 ^ (std::hash<int>{}(qp.cols) << 1);
			std::size_t h9 = h8 ^ (std::hash<string>{}(qp.fragsource) << 1);
			return h9;
		}
	};
}

class QuadHandler
{
	// maps the concatenation of vertex shader source file 
	// + fragment source file strings
	// to the program ID for that shader
	// used to check if a program for that already exists
	std::unordered_map<std::string, Shader> shaderIDs;
	// data to draw a quad
	std::vector<GLQuadData> data;
	// maps an entity to a vector of indices for each quad it has
	// and the index of the active quad
	std::unordered_map<uint64_t, std::pair<uint32_t, 
										   std::vector<uint32_t>>> Index;
	// maps a quad to its index to see if it's already in data		
	std::unordered_map<QuadParams, uint32_t> aliases;

	GLuint EBO;	// all quads use the same EBO
	ImageHandler* img_handler_;

public:
	// sets the active quad for the entity
	void SetActive(EntityID const, QuadParams const&);
	// generates textures and buffers and associates sets of quads with entity
	// first quad is the active quad by default
	void Add(EntityID const, std::vector<QuadParams>const &, 
							 uint32_t const activeQuad);
	// returns the GLQuadData for that animation state of that quad
	std::vector<GLQuadData> GetData(EntityID const handle);

	QuadHandler(ImageHandler*);
};
