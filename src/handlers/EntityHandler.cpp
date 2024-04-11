#include "EntityHandler.h"
#include "utility.h"

#include <string>
#include <stdexcept>

using std::vector, std::unordered_map, std::unordered_set;
//EntityHandler::EntityHandler(AnimationHandler* ah, Renderer* r,
//							 EventHandler* eh, MovementHandler* mh,
//							 ShaderHandler* sh, PositionHandler* ph) :
//	anim_handler_(ah), event_handler_(eh), move_handler_(mh), 
//	renderer_(r), shader_handler_(sh), position_handler_(ph)
//{
//	AddRenderer();
//}
//

template<>
EntityID EntityHandler::NewEntity(	EntityHandler::Character const& type) {


}

void EntityHandler::AddBehavior(EntityID handle, Behavior bhv) {

	switch (bhv) {
	case Behavior::Walk:
		EventBehaviorPriority& evt_bhv = entity_behaviors_[handle].event_priorities;
		AddBehaviorPriority(evt_bhv, Behavior::Walk,
			{
				{EventHandler::Event::KD_W, 0 },
				{EventHandler::Event::KD_A, 0 },
				{EventHandler::Event::KD_S, 0 },
				{EventHandler::Event::KD_D, 0 },
				{EventHandler::Event::KU_W, 0 },
				{EventHandler::Event::KU_A, 0 },
				{EventHandler::Event::KU_S, 0 },
				{EventHandler::Event::KU_D, 0 }
			});

		break;
	}

}

//EntityID EntityHandler::AddCharacter(PositionHandler::Quad quad_pos)
//{
//	auto handle = AddEntity(EntityType::Character);
//	// add components
//	anim_handler_->Add(handle,{ AnimationHandler::AnimType::PLAYERWALKUP, 
//								AnimationHandler::AnimType::PLAYERWALKDOWN, 
//								AnimationHandler::AnimType::PLAYERWALKLEFT, 
//								AnimationHandler::AnimType::PLAYERWALKRIGHT });
//	position_handler_->Add(handle, quad_pos);
//	event_handler_->Add(handle, {	EventHandler::Events::KD_W,
//									EventHandler::Events::KD_A,
//									EventHandler::Events::KD_S,
//									EventHandler::Events::KD_D });
//	move_handler_->Add(handle);
//	shader_handler_->Add(ShaderHandler::VertexShaders::Quad, 
//						 ShaderHandler::FragmentShaders::Quad, 
//						 handle);
//	return handle;
//}
//
//EntityID EntityHandler::AddOrthoCamera(PositionHandler::Quad position)
//{
//	auto handle = AddEntity(EntityType::OrthoCamera);
//	position_handler_->Add(handle, position);
//	return handle;
//}
//
//EntityID EntityHandler::AddRenderer() {
//	auto handle = AddEntity(EntityType::Renderer);
//	event_handler_->Add(handle, { EventHandler::Events::KD_ESC, 
//								  EventHandler::Events::QUIT });
//	return handle;
//}
//
//void EntityHandler::UpdateEvents()
//{
//	auto& notifications = event_handler_->GetEvents();
//	for (auto& notif : notifications) {
//		auto& handle = notif.first;
//		auto found = type_map_.find(handle);
//		if (found == type_map_.end()) {
//			string err = "entity " + to_string(handle) + 
//						 " not found in type_map";
//			throw runtime_error(err);
//		}
//		switch (found->second) {
//			case EntityType::Character: {
//				NotifyCharacter(notif.second, handle);
//				break;
//			}
//			case EntityType::Renderer: {
//				NotifyRenderer(notif.second, handle);
//				break;
//			}
//		}
//	}
//	event_handler_->ClearEvents();
//}
//
//
//void EntityHandler::NotifyRenderer(vector<EventHandler::Events> const& evts, 
//								   EntityID const handle)
//{
//	for (auto& evt : evts) {
//		switch (evt) {
//			case EventHandler::Events::KD_ESC: {
//				renderer_->alive = false;
//				break;
//			}
//			case EventHandler::Events::QUIT: {
//				renderer_->alive = false;
//				break;
//			}
//		}
//	}
//}


EntityHandler::EntityHandler()
{
	event_handler_ = std::make_unique<EventHandler>();
	timer_ = std::make_unique<Timer>();
	shader_handler_ = std::make_unique<ShaderHandler>();
	position_handler_ = std::make_unique<PositionHandler>();
	move_handler_ = std::make_unique<MovementHandler>(*timer_, *position_handler_);
	img_handler_ = std::make_unique<ImageHandler>();
	quad_handler_ = std::make_unique<QuadHandler>(*img_handler_);
	anim_handler_ = std::make_unique<AnimationHandler>(*quad_handler_, *position_handler_);

}

void EntityHandler::BehaviorEvent(	EntityID const& handle,
									EventHandler::Event const& event, 
									Behavior const& behavior)
{
	switch (behavior) {
	case EntityHandler::Behavior::Controllable:
		ControllableEvent(handle, event);
		break;
	}
}

void EntityHandler::ControllableEvent(	EntityID const& handle,
										EventHandler::Event const& evt)
{
	glm::vec3 input_vec;
	switch (evt) {
	case EventHandler::Event::KD_W: {
		input_vec.y = 1;
		event_handler_->Remove(handle, { EventHandler::Event::KD_W });
		event_handler_->Add(handle, { EventHandler::Event::KU_W });
		anim_handler_->SetActive(handle,
			AnimationHandler::AnimType::PLAYERWALKUP);
		break;
	}
	case EventHandler::Event::KD_A: {
		input_vec.x = -1;
		event_handler_->Remove(handle, { EventHandler::Event::KD_A });
		event_handler_->Add(handle, { EventHandler::Event::KU_A });
		anim_handler_->SetActive(handle,
			AnimationHandler::AnimType::PLAYERWALKLEFT);
		break;
	}
	case EventHandler::Event::KD_S: {
		input_vec.y = -1;
		event_handler_->Remove(handle, { EventHandler::Event::KD_S });
		event_handler_->Add(handle, { EventHandler::Event::KU_S });
		anim_handler_->SetActive(handle,
			AnimationHandler::AnimType::PLAYERWALKDOWN);
		break;
	}
	case EventHandler::Event::KD_D: {
		input_vec.x = 1;
		event_handler_->Remove(handle, { EventHandler::Event::KD_D });
		event_handler_->Add(handle, { EventHandler::Event::KU_D });
		anim_handler_->SetActive(handle,
			AnimationHandler::AnimType::PLAYERWALKRIGHT);
		break;
	}
	case EventHandler::Event::KU_W: {
		input_vec.y += 0;
		event_handler_->Remove(handle, { EventHandler::Event::KU_W });
		event_handler_->Add(handle, { EventHandler::Event::KD_W });
		break;
	}
	case EventHandler::Event::KU_A: {
		input_vec.x = 0;
		event_handler_->Remove(handle, { EventHandler::Event::KU_A });
		event_handler_->Add(handle, { EventHandler::Event::KD_A });
		break;
	}
	case EventHandler::Event::KU_S: {
		input_vec.y = 0;
		event_handler_->Remove(handle, { EventHandler::Event::KU_S });
		event_handler_->Add(handle, { EventHandler::Event::KD_S });
		break;
	}
	case EventHandler::Event::KU_D: {
		input_vec.x = 0;
		event_handler_->Remove(handle, { EventHandler::Event::KU_D });
		event_handler_->Add(handle, { EventHandler::Event::KD_D });
		break;
		}
	}
	move_handler_->SetVelocity(handle, glm::normalize(input_vec));
}

void EntityHandler::MakeEventPriority(	EventBehaviorPriority& ebp, 
										EventHandler::Event evt,
										vector<Behavior> const& behavior_priority)
{
	for (auto& bhv : behavior_priority) {
		ebp[evt].push_back(bhv);
	}
}

void EntityHandler::AddBehaviorPriority(
	EventBehaviorPriority& ebp,
	Behavior bhv,
	unordered_map<EventHandler::Event, u32> const& evts) 
{
	for (auto& evt_prio : evts) {
		ebp[evt_prio.first].insert(evt_prio.second, bhv);
	}
}

EntityID EntityHandler::AddEntity()
{
	if (this->used_handles_.size() == 0)
	{
		++highest_count_;
		return highest_count_;
	}
	else {
		auto max_used = used_handles_.top();
		used_handles_.pop();
		return max_used;
	}
}

void EntityHandler::RemoveEntity(EntityID handle)
{
	if (handle < highest_count_) {
		used_handles_.emplace(handle);
	}
	else {
		// if handle is highest_count_ then decrement until available handle
		--highest_count_;
		while (highest_count_ == used_handles_.top()) {
			--highest_count_;
			used_handles_.pop();
		}
	}
}
