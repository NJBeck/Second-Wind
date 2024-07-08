
#include "handlers/EntityHandler.h"

int main(int argc, char* args[]) {
    EntityHandler entity_handler;
    auto Player = entity_handler.MakePlayer({ Pos{0.0}, Dims{1.0}, Dir{0.0} });
    auto Renderer = entity_handler.MakeRenderer();
    auto Camera = entity_handler.MakeCamera({ { 0.0f, 0.0f, 1.0f}, 
        { 16.0f, 9.0f, 0.0f }, Dir{ 0.0f} });
    entity_handler.ActiveCam(Camera);
    while (entity_handler.Alive()) {
        entity_handler.Update();
    }

    SDL_Quit();
	return 0;
}