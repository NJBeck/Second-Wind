#include "QuadHandler.h"
#include "utility.h"

#define glCheckError() utility::glCheckError_(__FILE__, __LINE__) 

using std::string, std::array, std::vector, std::pair;

void QuadHandler::Add(EntityID const handle, 
                      vector<QuadParams> const& params, 
                      uint32_t activeQuad = 0)
{

    // indices for GLQuadData for the quads of this entity
    vector<uint32_t> QuadIndices;
    QuadIndices.reserve(params.size());

    // for each quad passed in we generate the shaders and buffers etc
    // then we put the data in the maps
    for (auto& param : params) {
        string image_path = utility::getDataPath(param.image_path);
        // if this quad isn't already in the vector
        auto alias = aliases.find(param);
        if (alias == aliases.end()) {

            // register handle with image
            img_handler_->Add(handle, image_path);
            auto& imd = img_handler_->GetImgData(image_path);



            // vertex array
            array<float, 16> verts;
            verts = {
                param.width,	0.0f,	        (float)(param.col + 1) / param.cols,    (float)(param.row) / param.rows,	// bottom right
                0.0f,	        0.0f,	        (float)(param.col) / param.cols,		(float)(param.row) / param.rows,	// bottom left
                param.width,    param.height,   (float)(param.col + 1) / param.cols,	(float)(param.row + 1) / param.rows,// top right
                0.0f,	        param.height,   (float)(param.col) / param.cols,		(float)(param.row + 1) / param.rows	// top left
            };

            // generate our buffers/textures objects
            uint32_t VBO, VAO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glCheckError();

            // bind and load vertex and element buffers
            glBindVertexArray(VAO);
            glCheckError();

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glCheckError();
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_STATIC_DRAW);
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
            uint32_t texture;
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
            // check if the shader for this quad has already been created
            string const src = param.vertsource + param.fragsource;
            auto shaderPtr = shaderIDs.find(src);
            if (shaderPtr == shaderIDs.end()) {
                auto vertsource = utility::getDataPath(param.vertsource);
                auto fragsource = utility::getDataPath(param.fragsource);
                Shader quadshader(vertsource, fragsource );
                shaderIDs.emplace(src, quadshader);
                GLQuadData glqd = { quadshader , texture, VAO, VBO, EBO };
                data.push_back(glqd);
            }
            else {
                GLQuadData glqd = { shaderPtr->second , texture, VAO, VBO, EBO };
                data.push_back(glqd);
            }
            QuadIndices.push_back(data.size() - 1);
            aliases[param] = data.size() - 1;
        }
        else {
            // if the quad was already in data then just push the index
            QuadIndices.push_back(alias->second);
        }

    }
    pair<uint32_t, vector<uint32_t>> _pair = { activeQuad, QuadIndices };
    Index[handle] = _pair;

}

vector<GLQuadData> QuadHandler::GetData(EntityID const handle){
    vector<GLQuadData> quaddata;
    auto found = Index.find(handle);
    if (found != Index.end()) {
        quaddata.emplace_back(data[found->second.first]);
    }
    return quaddata;
}

QuadHandler::QuadHandler(ImageHandler* imh): img_handler_(imh)
{
    // index array
    array<int, 6> indices{ {
    0, 1, 3, // first triangle
    0, 2, 3  // second triangle
    } };
    glGenBuffers(1, &EBO);
    glCheckError();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glCheckError();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
    glCheckError();
}


void QuadHandler::SetActive(EntityID const handle, QuadParams const& params) {
    auto const alias = aliases.find(params);
    if (alias != aliases.end()) {
        Index[handle].first = alias->second;
    }
    // might want to do some logging if it's not found
}

bool operator==(const QuadParams& lhs, const QuadParams& rhs) {
    if (
        lhs.image_path == rhs.image_path &&
        lhs.vertsource == rhs.vertsource &&
        lhs.fragsource == rhs.fragsource &&
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

