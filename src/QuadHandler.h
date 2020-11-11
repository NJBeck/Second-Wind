#pragma once
// packages some image data, screen space and shaders into 1 object

// TODO: handle entity deletion/garbage collection
//		 add more quads per entity
//		 should probably simplify the add() function call arguments

#include <string>
#include <map>
#include <array>

#include "ImageHandler.h"
#include "shader.h"

// the data necessary to draw the quad (other than the image itself)
struct GLQuadData {
	Shader shaders;		// ID for generated shader program
	GLuint texture;		// texture ID
	GLuint VAO;			// vertex array object for quad
	GLuint VBO;			// vertex buffer object
	GLuint EBO;			// element buffer object
};

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


class QuadHandler
{
	ImageHandler* ih;
	std::vector<GLQuadData> data;		// data for quads
	// maps an entity to index in data for its quad
	std::unordered_map<unsigned long, long> Index;

public:
	QuadHandler(ImageHandler* ihandler): ih(ihandler){}

	// generates textures and buffers and associates image with entity
	void add(unsigned long, QuadParams);
	bool HasQuad(unsigned long handle);
	GLQuadData GetData(unsigned long handle);
};

