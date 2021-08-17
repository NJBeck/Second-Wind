// TODO: use uniform to scale quads appropriately
#include "RENDER2D.h"
#include "utility.h"
#include "globals.h"

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
    camera = { {0.0, 0.0, 16.0, 9.0}, 1280, 720 };
    // initializing SDL, GLAD and make window and context
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        LATEST_SDL_ERROR
        SDL_Quit();
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    window = SDL_CreateWindow("Second Wind", SDL_WINDOWPOS_CENTERED, 
                               SDL_WINDOWPOS_CENTERED, camera.w_width,
                               camera.w_height, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        LATEST_SDL_ERROR
    }
    SDL_GLContext GLcontext = SDL_GL_CreateContext(window);
    if (!gladLoadGL()) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    SDL_GL_SetSwapInterval(1);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void Renderer::DrawScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // get the position info for the entities that are within range
    auto in_range = pos_handler_->EntitiesInArea(camera.quad);

    // vectors to put the quads and positions into
    vector<tuple<EntityHandler::ID, PositionHandler::Quad, GLQuadData> > ent_data;
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
        float screenX = 2 * (position.xPos - camera.quad.xPos) / camera.quad.width - 1;
        float screenY = 2 * (position.yPos - camera.quad.yPos) / camera.quad.height - 1;

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
}
