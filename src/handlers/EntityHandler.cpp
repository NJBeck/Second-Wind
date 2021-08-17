#include "entity.h"

#include <string>
#include <stdexcept>

using namespace std;

EntityHandler::EntityHandler(AnimationHandler* ah, Renderer* r,
							 EventHandler* eh, MovementHandler* mh) :
	anim_handler_(ah), event_handler_(eh), move_handler_(mh), renderer_(r)
{
	pos_handler_ = move_handler_->pos_handler_;
	quad_handler_ = anim_handler_->quad_handler_;
	count = 0;
}

void EntityHandler::AddCharacter(PositionHandler::Quad quad_pos)
{
	++count;
	type_map_[count] = Type::Character;
	// generate the quad params for the character
	vector<QuadParams> quads;
	string img{ "walkcycle/BODY_male.png" };
	string vs{ "quadshader.vs" };
	string fs{ "quadshader.fs" };
	float width = quad_pos.width * 2;
	float height = quad_pos.height * 2;
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 4; ++j) {
			quads.push_back({ img, vs, fs,
								width,
								height,
								j, 4,
								i, 9 });
		}
	}
	// add components
	// 33 should be the index for the first PLAYERWALKDOWN quad
	quad_handler_->Add(count, quads, 33);
	anim_handler_->Add(count,
		{ AnimType::PLAYERWALKUP, AnimType::PLAYERWALKDOWN, 
		  AnimType::PLAYERWALKLEFT, AnimType::PLAYERWALKRIGHT });
	pos_handler_->Add(count, quad_pos);
	event_handler_->Add(count, EventHandler::Events::KD_W);
	event_handler_->Add(count, EventHandler::Events::KD_A);
	event_handler_->Add(count, EventHandler::Events::KD_S);
	event_handler_->Add(count, EventHandler::Events::KD_D);
	move_handler_->Add(count);
}

void EntityHandler::UpdateEvents()
{
	auto& notifications = event_handler_->GetEvents();
	for (auto& notif : notifications) {
		auto& handle = notif.first;
		auto found = type_map_.find(handle);
		if (found == type_map_.end()) {
			string err = "entity " + to_string(handle) + " not found in type_map";
			throw runtime_error(err);
		}
		switch (found->second) {
		case Type::Character:
			NotifyCharacter(notif.second, handle);
			break;
		}
	}
}

void EntityHandler::AddRenderer() {
	++count;
	event_handler_->Add(count, EventHandler::Events::KD_ESC);
	event_handler_->Add(count, EventHandler::Events::QUIT);
}

void EntityHandler::NotifyCharacter(vector<EventHandler::Events> const& evts, 
									ID const handle)
{
	int inputXDir = 0;
	int inputYDir = 0;
	for (auto& evt : evts) {
		switch (evt) {
		case EventHandler::Events::KD_W: {
			inputYDir += 1;
			event_handler_->Remove(handle, EventHandler::Events::KD_W);
			event_handler_->Add(handle, EventHandler::Events::KU_W);
			anim_handler_->SetActive(handle, AnimType::PLAYERWALKUP);
			break;
			}
		case EventHandler::Events::KD_A: {
			inputXDir += -1;
			event_handler_->Remove(handle, EventHandler::Events::KD_A);
			event_handler_->Add(handle, EventHandler::Events::KU_A);
			anim_handler_->SetActive(handle, AnimType::PLAYERWALKLEFT);
			break;
			}
		case EventHandler::Events::KD_S: {
			inputYDir += -1;
			event_handler_->Remove(handle, EventHandler::Events::KD_S);
			event_handler_->Add(handle, EventHandler::Events::KU_S);
			anim_handler_->SetActive(handle, AnimType::PLAYERWALKDOWN);
			break;
			}
		case EventHandler::Events::KD_D: {
			inputXDir += 1;
			event_handler_->Remove(handle, EventHandler::Events::KD_D);
			event_handler_->Add(handle, EventHandler::Events::KU_D);
			anim_handler_->SetActive(handle, AnimType::PLAYERWALKRIGHT);
			break;
			}
		case EventHandler::Events::KU_W: {
			inputYDir += -1;
			event_handler_->Remove(handle, EventHandler::Events::KU_W);
			event_handler_->Add(handle, EventHandler::Events::KD_W);
			break;
			}
		case EventHandler::Events::KU_A: {
			inputXDir += 1;
			event_handler_->Remove(handle, EventHandler::Events::KU_A);
			event_handler_->Add(handle, EventHandler::Events::KD_A);
			break;
			}
		case EventHandler::Events::KU_S: {
			inputYDir += 1;
			event_handler_->Remove(handle, EventHandler::Events::KU_S);
			event_handler_->Add(handle, EventHandler::Events::KD_S);
			break;
			}
		case EventHandler::Events::KU_D: {
			inputXDir += -1;
			event_handler_->Remove(handle, EventHandler::Events::KU_D);
			event_handler_->Add(handle, EventHandler::Events::KD_D);
			break;
			}
		}
	}
	move_handler_->SetVelocity(handle, (double)inputXDir, (double)inputYDir);

}

void EntityHandler::NotifyRenderer(std::vector<EventHandler::Events> const& evts, ID const handle)
{
	for (auto& evt : evts) {
		switch (evt) {
			case EventHandler::Events::KD_ESC: {
				SDL_Quit();
				renderer_->alive = false;
				break;
			}
			case EventHandler::Events::QUIT: {
				SDL_Quit();
				renderer->alive = false;
				break;
			}
		}
	}
}
