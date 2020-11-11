#include "RENDER2D.h"
#include "utility.h"

#include <iostream>
#include <array>
#include <algorithm>
#include <vector>

using std::string, std::cout, std::endl, std::array, std::vector;
using namespace utility;

#define glCheckError() glCheckError_(__FILE__, __LINE__) 

RENDER2D::RENDER2D(SDL_Window* wind, OrthoCam ocam, QuadHandler* qh, PositionHandler* ph)
    :window(wind), cam(ocam), qHandler(qh), posHandler(ph)
{
    window = SDL_CreateWindow("Second Wind", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               cam.w_width, cam.w_height, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        LATEST_SDL_ERROR
    }
    SDL_GLContext GLcontext = SDL_GL_CreateContext(window);
    if (!gladLoadGL()) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void RENDER2D::DrawScene()
{

    glClear(GL_COLOR_BUFFER_BIT);

    // get the entities whose position falls inside our camera
    vector<unsigned long> entitiesInRange = posHandler->EntitiesInRanges(cam.left, cam.left + cam.width, 
                                                                         cam.bottom, cam.bottom + cam.height);
    // if it has quad component then draw it
    for (auto& ent : entitiesInRange) {
        if (qHandler->HasQuad(ent)) {
            // retrieve data for quad
            GLQuadData quad = qHandler->GetData(ent);

            // specify texture unit
            glActiveTexture(GL_TEXTURE0);
            glCheckError();

            // bind the quad's texture
            glBindTexture(GL_TEXTURE_2D, quad.texture);
            glCheckError();

            // use its shader program
            quad.shaders.use();
            glCheckError();

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
    }
    SDL_GL_SwapWindow(window);

}

RENDER2D::~RENDER2D() {
    SDL_DestroyWindow(window);
}
