#include "QuadHandler.h"
#include "utility.h"

#define glCheckError() utility::glCheckError_(__FILE__, __LINE__) 

using std::string, std::array;

void QuadHandler::add(unsigned long handle, QuadParams params)
{
	// register handle with image
	ih->add(handle, utility::getDataPath(params.imagePath));
    ImageData imd = ih->GetData(handle);

    // index array
	array<int, 6> indices{ {
	0, 1, 3, // first triangle
	0, 2, 3  // second triangle
	} };

    // vertex array
	array<float, 16> verts;
	verts = {
		params.width,	0.0f,	        (float)(params.col + 1) / params.cols,  (float)(params.row) / params.rows,	// bottom right
		0.0f,	        0.0f,	        (float)(params.col) / params.cols,		(float)(params.row) / params.rows,	// bottom left
		params.width,   params.height,  (float)(params.col + 1) / params.cols,	(float)(params.row + 1) / params.rows,// top right
		0.0f,	        params.height,  (float)(params.col) / params.cols,		(float)(params.row + 1) / params.rows	// top left
	};

    // generate our buffers/textures objects
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glCheckError();

    // bind and load vertex and element buffers
    glBindVertexArray(VAO);
    glCheckError();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glCheckError();
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_STATIC_DRAW);
    glCheckError();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glCheckError();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
    glCheckError();
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glCheckError();
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glCheckError();
    // configure texture 
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glCheckError();
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glCheckError();
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glCheckError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glCheckError();
    // load textures and generate mipmaps
    if (imd.channels == 3) {
        //glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, 
        //GLsizei height, GLint border, GLenum format, GLenum type, const void* data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imd.width, imd.height, 0, GL_RGB, GL_UNSIGNED_BYTE, imd.dataPtr);
        glCheckError();
        glGenerateMipmap(GL_TEXTURE_2D);
        glCheckError();
    }
    else {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imd.width, imd.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imd.dataPtr);
        glCheckError();
        glGenerateMipmap(GL_TEXTURE_2D);
        glCheckError();
    }

	// if it's already in the vector/index then just change it in place
	auto search = Index.find(handle);
	if (search != Index.end()){
		data[search->second] = { params.shaders, texture, VAO, VBO, EBO };
	}
	else {
	    // put data in data vector and add handle to index
	    data.push_back({ params.shaders, texture, VAO, VBO, EBO });
	    Index[handle] = data.size() - 1;
	}
}

bool QuadHandler::HasQuad(unsigned long handle) {
	auto search = Index.find(handle);
	if (search != Index.end()) return true;
	else return false;
}

GLQuadData QuadHandler::GetData(unsigned long handle){
    return data[Index[handle]];
}