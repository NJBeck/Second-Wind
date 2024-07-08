#include "QuadHandler.h"

using std::string, std::array, std::vector, std::unique_ptr, std::unordered_set;

void QuadHandler::Add(EntityID const handle, 
                      vector<QuadParams> const& params)
{
    auto& ent_data = index_[handle];
    // register images first
    unordered_set<ImageHandler::Image> imgs;
    for (auto& param : params) {
        imgs.emplace(param.image);
    }
    image_handler_.Add(handle, imgs);

    for (auto& param : params) {
        auto& mat = quad_mats_[static_cast<u32>(param.image)]
                        .operator()(param.rows, param.columns);
        // if quad doesnt have a VAO then make one
        if (mat.count == 0) {
            auto& img_data = image_handler_.GetImageData(param.image);
            // vertex array with texture coordinates
            array<float, 20> verts;
            verts = {
                0.5f, -0.5f, 0.0f,
                (float)(param.columns + 1) / img_data.columns,    
                (float)(param.rows) / img_data.rows,	// bottom right
                -0.5f, -0.5f, 0.0f,
                (float)(param.columns) / img_data.columns,		
                (float)(param.rows) / img_data.rows,	// bottom left
                0.5f, 0.5f, 0.0f,
                (float)(param.columns + 1) / img_data.columns,	
                (float)(param.rows + 1) / img_data.rows,// top right
                -0.5f, 0.5f, 0.0f,
                (float)(param.columns) / img_data.columns,
                (float)(param.rows + 1) / img_data.rows	// top left
            };

            // generate our buffers/textures objects
            GLuint VBO, VAO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            //glCheckError();

            // bind and load vertex and element buffers
            glBindVertexArray(VAO);
            //glCheckError();

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            //glCheckError();
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, 
                            GL_STATIC_DRAW);
            //glCheckError();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
            // position attribute
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(  0, 3, GL_FLOAT, GL_FALSE, 
                                    5 * sizeof(float), (void*)0);
            //glCheckError();
            // texture coord attribute
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(  1, 2, GL_FLOAT, GL_FALSE, 
                                    5 * sizeof(float), (void*)(3 * sizeof(float)));
            //glCheckError();

            // store the VAO value in the (row, column) of the matrix for this image
            mat.VAO = VAO;
            glBindVertexArray(0);
        }
        mat.count++;    
        ent_data.VAO_set.emplace(mat.VAO);
    }
}

void QuadHandler::Remove(EntityID const handle, std::vector<QuadParams> const& params)
{
    // should probably add a case for if removing active quad
    auto& entity_data = index_[handle];
    for (auto& param : params) {
        auto& mat = quad_mats_[static_cast<u32>(param.image)]
                        .operator()(param.rows, param.columns);
        if (mat.count == 1) {
            // Unbind the VAO and VBO before deleting them
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Delete the VAO
            glDeleteVertexArrays(1, &mat.VAO);
        }
        entity_data.VAO_set.erase(mat.VAO);
        mat.count--;
    }
}

std::optional<QuadHandler::GLQuadData> const
QuadHandler::GetActiveData(EntityID const handle) const{
    auto found_iter = index_.find(handle);
    if (found_iter != index_.end()) {
        GLQuadData ent_data;
        auto found_data = found_iter->second;
        ent_data.VAO = found_data.active_VAO;
        ent_data.texture = image_handler_.GetActiveTexture(handle);
        return ent_data;
    }
    else {
        return std::nullopt;
    }
}

QuadHandler::QuadHandler(ImageHandler& image_handler)
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
    //glCheckError();

    // construct matrices to store the data for each sprite in use
    for (auto& img_data : image_handler_.image_data) {
        quad_mats_.push_back( Matrix<QuadData>(img_data.rows, img_data.columns));
    }
}


void QuadHandler::SetActive(EntityID const handle, 
                            QuadParams const& params) {
    auto found_iter = index_.find(handle);
    if(found_iter == index_.end()){
        string err{ "entity " + std::to_string(handle) + 
                    " not found in QuadHandler when setting active" };
        throw std::runtime_error(err);
    }
    // check if there's a VAO for these params already
    auto& quad_data = quad_mats_[static_cast<u32>(params.image)]
                        .operator()(params.rows, params.columns);
    if (quad_data.count == 0) { // if no VAO for this param then make one
        Add(handle, { params });
    }
    auto& found = found_iter->second;
    found.active_VAO = quad_data.VAO;
    found.VAO_set.emplace(quad_data.VAO);
    image_handler_.SetActive(handle, params.image);
}
