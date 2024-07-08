
#include "Renderer.h"
#include "utility.h"

#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#include <tuple>
#include "glm/ext.hpp"

using std::vector, std::tuple, std::get;
using namespace utility;

//#define //glCheckError() //glCheckError_(__FILE__, __LINE__) 

Renderer::Renderer( PositionHandler& ph, QuadHandler& qh, 
                    AnimationHandler& ah, ShaderHandler& sh,
                    SDL_Window* wind):
        pos_handler_(ph), quad_handler_(qh), 
        anim_handler_(ah), shader_handler_(sh), window_(wind)
{

}

void Renderer::DrawScene()
{
    GenCameraDrawData(active_cam_);

    auto cam_dat = index_[active_cam_];
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto& draw_dat : draw_data_) {
        //glCheckError();

        // use its shader program
        glUseProgram(draw_dat.shader);
        //glCheckError();

        glUniform1i(glGetUniformLocation(draw_dat.shader, "texture1"), 0);
        //glCheckError();

        // specify texture unit
        glActiveTexture(GL_TEXTURE0);
        //glCheckError();

        // bind the quad's texture
        glBindTexture(GL_TEXTURE_2D, draw_dat.texture);
        //glCheckError();

        // uniforms for vertex shader
        glUniformMatrix4fv( glGetUniformLocation(draw_dat.shader, "model"), 
                            1, GL_FALSE, &draw_dat.model_mat[0][0]);
        glUniformMatrix4fv( glGetUniformLocation(draw_dat.shader, "view"),
                            1, GL_FALSE, &cam_dat.view[0][0]);
        glUniformMatrix4fv( glGetUniformLocation(draw_dat.shader, "projection"),
                            1, GL_FALSE, &cam_dat.projection[0][0]);

        // bind the quad's VAO
        glBindVertexArray(draw_dat.VAO);
        //glCheckError();

        // enable blending for the alpha
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glCheckError();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glCheckError();
    }
    SDL_GL_SwapWindow(window_);
    draw_data_.clear();
}

glm::mat4 Renderer::MakeViewMat(EntityID const cam_handle, glm::vec3 const look_at)
{
    glm::vec3 cam_pos = pos_handler_.GetEntityBox(cam_handle).pos;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    return glm::lookAt(cam_pos, look_at, up);
}

void Renderer::GenCameraDrawData(EntityID const cam)
{
    utility::CheckMapForEntity(cam, index_);
    CamData cam_data_ = index_[cam];
    PositionHandler::Box cam_pos_data = pos_handler_.GetEntityBox(cam);
    auto entity_pos_data = pos_handler_.GetEntityBoxes(cam_pos_data);
    for (auto& ent_pos : entity_pos_data) {
        DrawData ent_draw_data;
        ent_draw_data.model_mat 
            = glm::translate(ent_draw_data.model_mat, ent_pos.second.pos);
        if (auto quad_data = quad_handler_.GetActiveData(ent_pos.first)) {
            ent_draw_data.texture = quad_data->texture;
            ent_draw_data.VAO = quad_data->VAO;
            ent_draw_data.shader = shader_handler_.GetActiveProgram(ent_pos.first);
            draw_data_.push_back(ent_draw_data);
        }
    }
}


void Renderer::Exit() {
    SDL_DestroyWindow(window_);
    window_ = NULL;
}

void Renderer::AddOrthoCamera(EntityID const handle, float width,
                        float height, glm::vec3 const look_at) {
    CamData cam_data;
    cam_data.view = MakeViewMat(handle, look_at);
    cam_data.projection = glm::ortho(-width / 2, width / 2,
        -height / 2, height / 2);
    index_[handle] = cam_data;
}

void Renderer::ActiveCam(EntityID const cam)
{
    active_cam_ = cam;
}
