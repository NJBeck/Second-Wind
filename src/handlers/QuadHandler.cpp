#include "QuadHandler.h"
#include "utility.h"

#define glCheckError() utility::glCheckError_(__FILE__, __LINE__) 

using   std::string, std::array, std::vector,
        std::pair, std::tuple;

void QuadHandler::Add(EntityID const handle, 
                      vector<QuadParams> const& params, 
                      uint32_t activeQuad)
{
    // indices for GLQuadData for the quads of this entity
    vector<uint32_t> QuadIndices;
    QuadIndices.reserve(params.size());

    // for each quad passed in we generate the buffers
    // then we put the data in the maps
    for (auto& param : params) {
        // if this quad isn't already in the vector
        tuple<ImageHandler::Image, int, int> param_tuple{   param.image, 
                                                            param.row, 
                                                            param.col };
        auto alias = aliases_.find(param_tuple);
        if (alias == aliases_.end()) {

            // register handle with image
            img_handler_->Add(handle, param.image);
            auto& imd = img_handler_->GetImageData(param.image);
            auto img_ptr = img_handler_->GetImagePtr(param.image);

            // vertex array with texture coordinates
            array<float, 16> verts;
            verts = {
                1.0f,	0.0f,	        
                (float)(param.col + 1) / imd.columns,    
                (float)(param.row) / imd.rows,	// bottom right
                0.0f, 0.0f,	        
                (float)(param.col) / imd.columns,		
                (float)(param.row) / imd.rows,	// bottom left
                1.0f, 1.0f,   
                (float)(param.col + 1) / imd.columns,	
                (float)(param.row + 1) / imd.rows,// top right
                0.0f, 1.0f,   
                (float)(param.col) / imd.columns,		
                (float)(param.row + 1) / imd.rows	// top left
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
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, 
                         GL_STATIC_DRAW);
            glCheckError();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            // position attribute
            glVertexAttribPointer(  0, 2, GL_FLOAT, GL_FALSE, 
                                    4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glCheckError();
            // texture coord attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 
                                  (void*)(2 * sizeof(float)));
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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                            GL_LINEAR_MIPMAP_LINEAR);
            glCheckError();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glCheckError();
            // load textures and generate mipmaps
            if (imd.channels == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imd.width, imd.height, 
                             0, GL_RGB, GL_UNSIGNED_BYTE, img_ptr);
                glCheckError();
                glGenerateMipmap(GL_TEXTURE_2D);
                glCheckError();
            }
            else {

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imd.width, imd.height, 
                             0, GL_RGBA, GL_UNSIGNED_BYTE, img_ptr);
                glCheckError();
                glGenerateMipmap(GL_TEXTURE_2D);
                glCheckError();
            }
            GLdata_.push_back({ texture, VAO });
            QuadIndices.push_back(GLdata_.size() - 1);
            aliases_[param_tuple] = GLdata_.size() - 1;
        }
        else {
            // if the quad was already in data then just push the index
            QuadIndices.push_back(alias->second);
        }

    }
    index_[handle] = { activeQuad, QuadIndices };
}

QuadHandler::GLQuadData
QuadHandler::GetActiveData(EntityID const handle) const{
    auto found = index_.find(handle);
    if (found != index_.end()) {
        return GLdata_[found->second.first];
    }
    else {
        return { 0, 0 };
    }
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), 
                 &indices, GL_STATIC_DRAW);
    glCheckError();
}


void QuadHandler::SetActive(EntityID const handle, 
                            QuadParams const& params) {
    auto const alias = aliases_.find(params.GetTuple());
    if (alias != aliases_.end()) {
        index_[handle].first = alias->second;
    }
    else {
        string err{ "entity " + std::to_string(handle) + 
                    " not found in QuadHandler when setting active" };
        throw std::runtime_error(err);
    }
}

tuple<ImageHandler::Image, int, int> QuadHandler::QuadParams::GetTuple() const
{
    tuple<ImageHandler::Image, int, int> tup{ image, row, col };
    return tup;
}
