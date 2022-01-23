#include "EntityHandler.h"

#include <string>
#include <stdexcept>

using namespace std;

EntityID EntityHandler::count = 0;

EntityHandler::EntityHandler(AnimationHandler* ah, Renderer* r,
							 EventHandler* eh, MovementHandler* mh,
							 ShaderHandler* sh, PositionHandler* ph) :
	anim_handler_(ah), event_handler_(eh), move_handler_(mh), 
	renderer_(r), shader_handler_(sh), position_handler_(ph)
{
	AddRenderer();
}

EntityID EntityHandler::AddCharacter(PositionHandler::Quad quad_pos)
{
	auto handle = AddEntity(EntityType::Character);
	// add components
	anim_handler_->Add(handle,{ AnimationHandler::AnimType::PLAYERWALKUP, 
								AnimationHandler::AnimType::PLAYERWALKDOWN, 
								AnimationHandler::AnimType::PLAYERWALKLEFT, 
								AnimationHandler::AnimType::PLAYERWALKRIGHT });
	position_handler_->Add(handle, quad_pos);
	event_handler_->Add(handle, {	EventHandler::Events::KD_W,
									EventHandler::Events::KD_A,
									EventHandler::Events::KD_S,
									EventHandler::Events::KD_D });
	move_handler_->Add(handle);
	shader_handler_->Add(ShaderHandler::VertexShaders::Quad, 
						 ShaderHandler::FragmentShaders::Quad, 
						 handle);
	return handle;
}

EntityID EntityHandler::AddOrthoCamera(PositionHandler::Quad position)
{
	auto handle = AddEntity(EntityType::OrthoCamera);
	position_handler_->Add(handle, position);
	return handle;
}

EntityID EntityHandler::AddRenderer() {
	auto handle = AddEntity(EntityType::Renderer);
	event_handler_->Add(handle, { EventHandler::Events::KD_ESC, 
								  EventHandler::Events::QUIT });
	return handle;
}

void EntityHandler::UpdateEvents()
{
	auto& notifications = event_handler_->GetEvents();
	for (auto& notif : notifications) {
		auto& handle = notif.first;
		auto found = type_map_.find(handle);
		if (found == type_map_.end()) {
			string err = "entity " + to_string(handle) + 
						 " not found in type_map";
			throw runtime_error(err);
		}
		switch (found->second) {
			case EntityType::Character: {
				NotifyCharacter(notif.second, handle);
				break;
			}
			case EntityType::Renderer: {
				NotifyRenderer(notif.second, handle);
				break;
			}
		}
	}
	event_handler_->ClearEvents();
}


EntityID EntityHandler::AddEntity(EntityType const ent_type)
{
	++count;
	type_map_[count] = ent_type;
	return count;
}

void EntityHandler::NotifyCharacter(vector<EventHandler::Events> const& evts,
									EntityID const handle)
{
	int inputXDir = 0;
	int inputYDir = 0;
	for (auto& evt : evts) {
		switch (evt) {
		case EventHandler::Events::KD_W: {
			inputYDir += 1;
			event_handler_->Remove(handle, { EventHandler::Events::KD_W });
			event_handler_->Add(handle, { EventHandler::Events::KU_W });
			anim_handler_->SetActive(handle, 
				AnimationHandler::AnimType::PLAYERWALKUP);
			break;
			}
		case EventHandler::Events::KD_A: {
			inputXDir += -1;
			event_handler_->Remove(handle, { EventHandler::Events::KD_A });
			event_handler_->Add(handle, { EventHandler::Events::KU_A });
			anim_handler_->SetActive(handle,
				AnimationHandler::AnimType::PLAYERWALKLEFT);
			break;
			}
		case EventHandler::Events::KD_S: {
			inputYDir += -1;
			event_handler_->Remove(handle, { EventHandler::Events::KD_S });
			event_handler_->Add(handle, { EventHandler::Events::KU_S });
			anim_handler_->SetActive(handle, 
				AnimationHandler::AnimType::PLAYERWALKDOWN);
			break;
			}
		case EventHandler::Events::KD_D: {
			inputXDir += 1;
			event_handler_->Remove(handle, { EventHandler::Events::KD_D });
			event_handler_->Add(handle, { EventHandler::Events::KU_D });
			anim_handler_->SetActive(handle, 
				AnimationHandler::AnimType::PLAYERWALKRIGHT);
			break;
			}
		case EventHandler::Events::KU_W: {
			inputYDir += -1;
			event_handler_->Remove(handle, { EventHandler::Events::KU_W });
			event_handler_->Add(handle, { EventHandler::Events::KD_W });
			break;
			}
		case EventHandler::Events::KU_A: {
			inputXDir += 1;
			event_handler_->Remove(handle, { EventHandler::Events::KU_A });
			event_handler_->Add(handle, { EventHandler::Events::KD_A });
			break;
			}
		case EventHandler::Events::KU_S: {
			inputYDir += 1;
			event_handler_->Remove(handle, { EventHandler::Events::KU_S });
			event_handler_->Add(handle, { EventHandler::Events::KD_S });
			break;
			}
		case EventHandler::Events::KU_D: {
			inputXDir += -1;
			event_handler_->Remove(handle, { EventHandler::Events::KU_D });
			event_handler_->Add(handle, { EventHandler::Events::KD_D });
			break;
			}
		}
	}
	move_handler_->SetVelocity(handle, 
							   { (double)inputXDir, (double)inputYDir });

}

void EntityHandler::NotifyRenderer(vector<EventHandler::Events> const& evts, 
								   EntityID const handle)
{
	for (auto& evt : evts) {
		switch (evt) {
			case EventHandler::Events::KD_ESC: {
				renderer_->alive = false;
				break;
			}
			case EventHandler::Events::QUIT: {
				renderer_->alive = false;
				break;
			}
		}
	}
}
