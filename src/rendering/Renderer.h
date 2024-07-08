#pragma once
#include "SDL2/SDL.h"
#include "glad/glad.h"

#include "handlers/PositionHandler.h"
#include "handlers/AnimationHandler.h"
#include "handlers/ShaderHandler.h"

//#define LATEST_SDL_ERROR std::cout << "SDL failure:" << \
//							SDL_GetError() << std::endl;


class Renderer
{
public:
	Renderer(PositionHandler&, QuadHandler&, AnimationHandler&,
			 ShaderHandler&, SDL_Window*);
	// draws entities located within the active camera's view
	void DrawScene();
	// width and height being the area covered in world units
	void AddOrthoCamera(EntityID const, float width, 
						float height, glm::vec3 const look_at );
	void ActiveCam(	EntityID const);
	void Exit();
private:
	void GenCameraDrawData(EntityID const cam);	// finds the entities in camera
	glm::mat4 MakeViewMat(EntityID const cam, glm::vec3 const look_at);
	struct DrawData {
		GLuint texture;
		GLuint VAO;
		GLuint shader;
		glm::mat4 model_mat{ 1.0f };
	};
	// stores the data for drawing an object
	std::vector<DrawData> draw_data_;
	struct CamData {
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
	};
	// stores the data for each camera 
	std::unordered_map<EntityID, CamData> index_;
	EntityID active_cam_;
	PositionHandler& pos_handler_;
	QuadHandler& quad_handler_;
	AnimationHandler& anim_handler_;
	ShaderHandler& shader_handler_;
	SDL_Window* window_;
};

