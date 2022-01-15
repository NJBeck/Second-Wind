// TODO: use uniform to scale quads appropriately
#include "Renderer.h"
#include "utility.h"

#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#include <tuple>

using namespace std;
using namespace utility;

#define glCheckError() glCheckError_(__FILE__, __LINE__) 

Renderer::Renderer(SDL_Window* w, PositionHandler* ph, 
                   QuadHandler* qh, AnimationHandler* ah)
    :window(w), pos_handler_(ph), quad_handler_(qh), anim_handler_(ah), 
    alive(true)
{
}

void Renderer::DrawScene()
{
    if (!alive) return;
    glClear(GL_COLOR_BUFFER_BIT);

    OrthoCam& camera = cameras_[0];

    PositionHandler::Quad camera_quad = { camera.xPos, camera.yPos,
                                          camera.width, camera.height };

    // get the position info for the entities that are within range
    auto in_range = pos_handler_->EntitiesInArea(camera_quad);

    // vectors to put the quads and positions into
    vector<tuple<EntityID, PositionHandler::Quad, GLQuadData> > ent_data;
    for (auto& ent : in_range) {
        // update the animation so the correct quad is selected
        anim_handler_->Update(ent.handle);
        // retrieve data for quads if it has one
        for (auto& quad : quad_handler_->GetData(ent.handle)) {
            ent_data.emplace_back(ent.handle, ent.quad, quad);
        }
    }

    for (uint32_t i = 0; i < ent_data.size(); ++i) {
        auto quad = get<2>(ent_data[i]);
        auto position = get<1>(ent_data[i]);
        auto ent_id = get<0>(ent_data[i]);
        // figure out where on the screen the quad should go
        float screenX = 2 * (position.xPos - camera_quad.xPos) 
                           / camera_quad.width - 1;
        float screenY = 2 * (position.yPos - camera_quad.yPos) 
                           / camera_quad.height - 1;
        // scale uniform to draw the entity at the right size on screen
        float scale = position.width / camera.width;

        // specify texture unit
        glActiveTexture(GL_TEXTURE0);
        glCheckError();

        // bind the quad's texture
        glBindTexture(GL_TEXTURE_2D, quad.texture);
        glCheckError();

        // use its shader program
        quad.shaders.use();
        glCheckError();

        quad.shaders.setVec2("screenPos", screenX, screenY);
        quad.shaders.setFloat("scale", scale);

        // specify the texture unit for the uniform
        quad.shaders.setInt("texture1", 0);
        glCheckError();

        // bind the quad's VAO
        glBindVertexArray(quad.VAO);
        glCheckError();

        // enable blending for the alpha
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glCheckError();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glCheckError();
    }
    SDL_GL_SwapWindow(window);
}


Renderer::~Renderer() {
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void Renderer::AddCamera(OrthoCam const& cam) {
    cameras_.emplace_back(cam);
}
