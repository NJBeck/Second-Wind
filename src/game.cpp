
#include "handlers/EntityHandler.h"
#include "rendering/Renderer.h"
#include "handlers/ImageHandler.h"
#include "handlers/ShaderHandler.hpp"
#include "utility.h"


int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    utility::InitGL(&window, 1280, 720);
    auto global_timer = std::make_unique<Timer>();
    auto img_handler = std::make_unique<ImageHandler>();
    auto quad_handler = std::make_unique<QuadHandler>(img_handler);
    PositionHandler::Quad map_boundaries;
    map_boundaries.width = (long)1 << 20;
    map_boundaries.height = map_boundaries.width;
    map_boundaries.xPos = -1 * map_boundaries.width / 2;  // bottom left corner
    map_boundaries.yPos = map_boundaries.xPos;
    auto pos_handler = std::make_unique<PositionHandler>(map_boundaries);
    auto anim_handler = std::make_unique<AnimationHandler>(quad_handler, pos_handler);
    auto move_handler = std::make_unique<MovementHandler>(pos_handler, global_timer);
    auto shader_handler = std::make_unique<ShaderHandler>();
    auto renderer = std::make_unique<Renderer>(window, &pos_handler, &quad_handler, 
                      &anim_handler, &shader_handler);
    renderer.AddCamera({{ 0.0, 0.0, 16.0, 9.0 }, 1280, 720});
    EventHandler event_handler;
    EntityHandler entity_handler(&anim_handler, &renderer, 
                                 &event_handler, &move_handler,
                                 &shader_handler, &pos_handler);
    entity_handler.AddCharacter({0.0, 0.0, 1.0, 1.0});
    while (renderer.alive) {
        global_timer.start();
        event_handler.PollEvents();
        entity_handler.UpdateEvents();
        move_handler.Update();
        renderer.DrawScene();
    }

	return 0;
}