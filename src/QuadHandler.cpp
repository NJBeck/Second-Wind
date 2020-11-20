#include "QuadHandler.h"
#include "utility.h"
#include "globals.h"

#define glCheckError() utility::glCheckError_(__FILE__, __LINE__) 

using std::string, std::array, std::vector;

void QuadHandler::add(unsigned long handle, vector<QuadParams> params)
{
    // indices for GLQuadData for the quads of this entity
    vector<unsigned int> QuadIndices;

    for (auto& param : params) {
        // if this quad isn't already in the vector
        auto alias = aliases.find(param);
        if (alias == aliases.end()) {

            // register handle with image
            globals::imgHandler.add(handle, utility::getDataPath(param.imagePath));
            ImageData& imd = globals::imgHandler.GetData(handle);

            // index array
            array<int, 6> indices{ {
            0, 1, 3, // first triangle
            0, 2, 3  // second triangle
            } };

            // vertex array
            array<float, 16> verts;
            verts = {
                param.width,	0.0f,	        (float)(param.col + 1) / param.cols,  (float)(param.row) / param.rows,	// bottom right
                0.0f,	        0.0f,	        (float)(param.col) / param.cols,		(float)(param.row) / param.rows,	// bottom left
                param.width,   param.height,  (float)(param.col + 1) / param.cols,	(float)(param.row + 1) / param.rows,// top right
                0.0f,	        param.height,  (float)(param.col) / param.cols,		(float)(param.row + 1) / param.rows	// top left
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
            data.push_back({ param.shaders, texture, VAO, VBO, EBO });
            QuadIndices.push_back(data.size() - 1);
            aliases[param] = data.size() - 1;
        }
        else {
            // if the quad was already in data then just push the index
            QuadIndices.push_back(alias->second);
        }

    }
    Index[handle] = QuadIndices;

}

bool QuadHandler::HasQuad(unsigned long handle) {
	auto search = Index.find(handle);
	if (search != Index.end()) return true;
	else return false;
}

vector<GLQuadData> QuadHandler::GetData(unsigned long handle){
    // look up the indices of the data and return the vector
    vector<GLQuadData> result;
    for (auto& _index : Index[handle]) {
        result.push_back(data[_index]);
    }
    return result;
}

bool operator==(const QuadParams& lhs, const QuadParams& rhs) {
    if (
        lhs.imagePath == rhs.imagePath &&
        lhs.shaders.ID == rhs.shaders.ID &&
        lhs.width == rhs.width &&
        lhs.height == rhs.height &&
        lhs.row == rhs.row &&
        lhs.rows == rhs.rows &&
        lhs.col == rhs.col &&
        lhs.cols == rhs.cols
        ) {
        return true;
    }
    else return false;

}

