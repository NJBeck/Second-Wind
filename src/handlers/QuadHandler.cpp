#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "QuadHandler.h"

using std::string, std::array, std::vector, std::unique_ptr, std::shared_ptr;

void QuadHandler::Add(EntityID const handle, 
                      vector<QuadParams> const& params)
{
    // register images first
    vector<ImageHandler::Image> imgs;
    for (auto& img : params) {
        imgs.push_back(img.image);
    }
    image_handler_->Add(handle, imgs);

    for (auto& param : params) {
        auto& mat = quad_mats_[static_cast<u32>(param.image)]
                        ->operator()(param.rows, param.columns);
        // if quad doesnt have a VAO then make one
        if (mat.count == 0) {
            auto& img_data = image_handler_->GetImageData(param.image);
            // vertex array with texture coordinates
            array<float, 16> verts;
            verts = {
                1.0f,	0.0f,	        
                (float)(param.columns + 1) / img_data.columns,    
                (float)(param.rows) / img_data.rows,	// bottom right
                0.0f, 0.0f,	        
                (float)(param.columns) / img_data.columns,		
                (float)(param.rows) / img_data.rows,	// bottom left
                1.0f, 1.0f,   
                (float)(param.columns + 1) / img_data.columns,	
                (float)(param.rows + 1) / img_data.rows,// top right
                0.0f, 1.0f,   
                (float)(param.columns) / img_data.columns,
                (float)(param.rows + 1) / img_data.rows	// top left
            };

            // generate our buffers/textures objects
            GLuint VBO, VAO;
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
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
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

            // store the VAO/VBO value in the (row, column) of the matrix for this image
            mat.VAO = VAO;
            mat.VBO = VBO;
        }
        mat.count++;
        index_[handle].VAO_set.emplace(mat.VAO);
    }
}

void QuadHandler::Remove(EntityID const handle, std::vector<QuadParams> const& params)
{
    // should probably add a case for if removing active quad
    auto& entity_data = index_[handle];
    for (auto& param : params) {
        auto& mat = quad_mats_[static_cast<u32>(param.image)]
                        ->operator()(param.rows, param.columns);
        if (mat.count == 1) {
            // Unbind the VAO and VBO before deleting them
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Delete the VBO
            glDeleteBuffers(1, &mat.VBO);

            // Delete the VAO
            glDeleteVertexArrays(1, &mat.VAO);
        }
        entity_data.VAO_set.erase(mat.VAO);
        mat.count--;
    }
}

QuadHandler::GLQuadData const
QuadHandler::GetActiveData(EntityID const handle) const{
    GLQuadData ent_data;
    auto& found_iter = index_.find(handle);
    if (found_iter != index_.end()) {
        auto found_data = found_iter->second;
        ent_data.VAO = found_data.active_VAO;
        ent_data.texture = image_handler_->GetImageData(found_data.image).ID;
        return ent_data;
    }
    else {
        string err{ "entity " + std::to_string(handle) +
                    " not found in QuadHandler when getting active VAO" };
        throw std::runtime_error(err);
    }
}

QuadHandler::QuadHandler(shared_ptr<ImageHandler> image_handler)
    : image_handler_(image_handler)
{
    // index array
    array<int, 6> indices{ {
    0, 1, 3, // first triangle
    0, 2, 3  // second triangle
    } };
    glGenBuffers(1, &EBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), 
                 &indices, GL_STATIC_DRAW);
    glCheckError();

    // construct matrices to store the data for each sprite in use
    for (auto& img_data : image_handler_->image_data) {
        auto i = static_cast<u32>(img_data.image);
        quad_mats_[i] = unique_ptr<Matrix<QuadData>>
                            (new Matrix<QuadData>(img_data.rows, img_data.columns));
    }
}


void QuadHandler::SetActive(EntityID const handle, 
                            QuadParams const& params) {
    auto& found_iter = index_.find(handle);
    if(found_iter == index_.end()){
        string err{ "entity " + std::to_string(handle) + 
                    " not found in QuadHandler when setting active" };
        throw std::runtime_error(err);
    }
    // check if there's a VAO for these params already
    auto& quad_data = quad_mats_[static_cast<u32>(params.image)]
                            ->operator()(params.rows, params.columns);
    if (quad_data.count == 0) { // if no VAO for this param then make one
        Add(handle, { params });
    }
    auto& found = found_iter->second;
    found.active_VAO = quad_data.VAO;
    found.VAO_set.emplace(quad_data.VAO);
    found.image = params.image;

}
