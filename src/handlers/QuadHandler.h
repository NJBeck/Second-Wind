#pragma once
// packages some image data, screen space and shaders into 1 object

// TODO:	handle entity deletion/garbage collection (probably use min heap of open indices)

#include <string>
#include <unordered_map>

#include "rendering/shader.h"

// the data necessary to draw the quad (other than the image itself)
struct GLQuadData {
	Shader shaders;		// ID for generated shader program
	GLuint texture;		// texture ID
	GLuint VAO;			// vertex array object for quad
	GLuint VBO;			// vertex buffer object
	GLuint EBO;			// element buffer object
};


// data to pass in for construction of a quad
struct QuadParams {
	std::string imagePath;	// filepath for image relative to /data/ folder
	Shader shaders;	// shader object for the quad
	float width;	// screenspace width of quad
	float height;
	int row;		// row of quad's texture in spritesheet
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
			std::size_t h1 = std::hash<std::string>{}(qp.imagePath);
			std::size_t h2 = h1 ^ (std::hash<unsigned int>{}(qp.shaders.ID) << 1);
			std::size_t h3 = h2 ^ (std::hash<float>{}(qp.width) << 1);
			std::size_t h4 = h3 ^ (std::hash<float>{}(qp.height) << 1);
			std::size_t h5 = h4 ^ (std::hash<int>{}(qp.row) << 1);
			std::size_t h6 = h5 ^ (std::hash<int>{}(qp.rows) << 1);
			std::size_t h7 = h6 ^ (std::hash<int>{}(qp.col) << 1);
			std::size_t h8 = h7 ^ (std::hash<int>{}(qp.cols) << 1);
			return h8;
		}
	};
}

class QuadHandler
{
	// data for quads
	std::vector<GLQuadData> data;
	// maps an entity to indices for its quads
	std::unordered_map<unsigned long, std::vector<unsigned int>> Index;
	// maps a quad to it's index to see if it's already in data		
	std::unordered_map<QuadParams, unsigned int> aliases;

public:

	// generates textures and buffers and associates image with entity
	void add(unsigned long, std::vector<QuadParams>);
	bool HasQuad(unsigned long handle);
	std::vector<GLQuadData> GetData(unsigned long handle);
};
