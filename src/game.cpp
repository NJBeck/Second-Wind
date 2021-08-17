
#include "handlers/EntityHandler.h"
#include "rendering/Renderer.h"
#include "handlers/ImageHandler.h"
#include "utility.h"


int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    Timer global_timer;
    ImageHandler img_handler;
    QuadHandler quad_handler(&img_handler);
    PositionHandler pos_handler({-1*1<<9, -1 * 1 << 9, 1 << 10,-1 << 10 });
    AnimationHandler anim_handler(&quad_handler, &pos_handler);
    MovementHandler move_handler(&pos_handler, &global_timer);
    Renderer renderer(window, &pos_handler, &quad_handler, &anim_handler);
    EventHandler event_handler;
    EntityHandler entity_handler(&anim_handler, &renderer, 
                                 &event_handler, &move_handler);
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