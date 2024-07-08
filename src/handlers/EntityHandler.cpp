#include "EntityHandler.h"
#include "utility.h"

#include <string>
#include <stdexcept>

using std::vector, std::unordered_map, std::unordered_set, std::deque;

EntityHandler::EntityHandler()
{
	highest_count_ = 0;
	SDL_Window* window_ = NULL;
	utility::InitGL(&window_, 1280, 720);
	event_handler_ = std::make_unique<EventHandler>();
	timer_ = std::make_unique<Timer>();
	shader_handler_ = std::make_unique<ShaderHandler>();
	PositionHandler::Box boundaries = { Pos(0.0f), {2000.0f, 2000.0f, 2000.0f}, Dir(0.0f) };
	position_handler_ = std::make_unique<PositionHandler>(boundaries);
	move_handler_ = std::make_unique<MovementHandler>(*position_handler_, *timer_);
	img_handler_ = std::make_unique<ImageHandler>();
	quad_handler_ = std::make_unique<QuadHandler>(*img_handler_);
	anim_handler_ = std::make_unique<AnimationHandler>(*quad_handler_, *position_handler_);
	renderer_ = std::make_unique<Renderer>(	*position_handler_, *quad_handler_, 
											*anim_handler_, *shader_handler_, window_);
	alive = true;
}

void EntityHandler::Remove(EntityID const handle)
{
	RemoveEntity(handle);
	utility::CheckMapForEntity(handle, index_);
	index_.erase(handle);
}

void EntityHandler::Update()
{
	event_handler_->PollEvents();
	timer_->start();
	// each frame has a vec of an event and the entities subscribed to it
	auto notification_frames = event_handler_->GetNotifications();
	for (auto& frame : notification_frames) {
		for (auto& evt_ents : frame) {
			for (auto ent : evt_ents.entities) {
				HandleEventBehavior(ent, evt_ents.event);
			}
		}
	}
	move_handler_->Update();
	anim_handler_->Update();
	event_handler_->ClearEvents();
	if (alive) {
		renderer_->DrawScene();
	}
}

bool EntityHandler::Alive()
{
	return alive;
}


void EntityHandler::AddBehavior(EntityID const handle, 
		vector<EventHandler::Event> const& evts, deque<Behavior> const& bhvs) 
{
	auto& ent_data = index_[handle];
	auto& evt_bhv = ent_data.event_behaviors;
	for (auto evt : evts) {
		evt_bhv[evt] = bhvs;
	}
}

void EntityHandler::HandleEventBehavior(EntityID const handle, 
										EventHandler::Event const evt){
	auto& ent_evt_bhv = index_[handle].event_behaviors;
	auto found_evt = ent_evt_bhv.find(evt);
	if (found_evt != ent_evt_bhv.end()) {
		auto& bhvs = found_evt->second;
		for (auto bhv : bhvs) {
			switch (bhv) {
			case Behavior::Controllable:
				ControllableEvent(handle, evt);
				break;
			case Behavior::Exit:
				ExitEvent();
				break;
			case Behavior::Walk:
				WalkEvent(handle);
				break;
			default: utility::Log("EntityHandler", "finding behavior", "for event");
				break;
			}
		}
	}
}


void EntityHandler::ControllableEvent(	EntityID const handle,
										EventHandler::Event const evt)
{
	glm::vec3 input_vec{ 0.0 };
	switch (evt) {
	case EventHandler::Event::KD_W: {
		input_vec.y = 1;
		event_handler_->Remove(handle, { EventHandler::Event::KD_W });
		event_handler_->Subscribe(handle, { EventHandler::Event::KU_W });
		anim_handler_->SetActive(handle,
			AnimationHandler::AnimType::PLAYERWALKUP);
		break;
	}
	case EventHandler::Event::KD_A: {
		input_vec.x = -1;
		event_handler_->Remove(handle, { EventHandler::Event::KD_A });
		event_handler_->Subscribe(handle, { EventHandler::Event::KU_A });
		anim_handler_->SetActive(handle,
			AnimationHandler::AnimType::PLAYERWALKLEFT);
		break;
	}
	case EventHandler::Event::KD_S: {
		input_vec.y = -1;
		event_handler_->Remove(handle, { EventHandler::Event::KD_S });
		event_handler_->Subscribe(handle, { EventHandler::Event::KU_S });
		anim_handler_->SetActive(handle,
			AnimationHandler::AnimType::PLAYERWALKDOWN);
		break;
	}
	case EventHandler::Event::KD_D: {
		input_vec.x = 1;
		event_handler_->Remove(handle, { EventHandler::Event::KD_D });
		event_handler_->Subscribe(handle, { EventHandler::Event::KU_D });
		anim_handler_->SetActive(handle,
			AnimationHandler::AnimType::PLAYERWALKRIGHT);
		break;
	}
	case EventHandler::Event::KU_W: {
		input_vec.y = -1;
		event_handler_->Remove(handle, { EventHandler::Event::KU_W });
		event_handler_->Subscribe(handle, { EventHandler::Event::KD_W });
		break;
	}
	case EventHandler::Event::KU_A: {
		input_vec.x = 1;
		event_handler_->Remove(handle, { EventHandler::Event::KU_A });
		event_handler_->Subscribe(handle, { EventHandler::Event::KD_A });
		break;
	}
	case EventHandler::Event::KU_S: {
		input_vec.y = 1;
		event_handler_->Remove(handle, { EventHandler::Event::KU_S });
		event_handler_->Subscribe(handle, { EventHandler::Event::KD_S });
		break;
	}
	case EventHandler::Event::KU_D: {
		input_vec.x = -1;
		event_handler_->Remove(handle, { EventHandler::Event::KU_D });
		event_handler_->Subscribe(handle, { EventHandler::Event::KD_D });
		break;
		}
	default: break;
	}
	move_handler_->AddDir(handle, input_vec);
}
void EntityHandler::WalkEvent(EntityID const handle)
{
	anim_handler_->ToUpdate(handle);
}
void EntityHandler::ExitEvent() { 
	renderer_->Exit();
	alive = false;
}

void EntityHandler::AddQuadShader(EntityID const handle)
{
	shader_handler_->Add(handle, ShaderHandler::VertexShader::QUAD,
		ShaderHandler::FragmentShader::QUAD);
	shader_handler_->SetActiveShader(handle, ShaderHandler::VertexShader::QUAD,
		ShaderHandler::FragmentShader::QUAD);
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

EntityID EntityHandler::MakeCharacter(PositionHandler::Box const pos,
									ShaderHandler::VertexShader const vs, 
									ShaderHandler::FragmentShader const fs,
									unordered_set<AnimationHandler::AnimType> const& anim_types)
{
	EntityID const handle = AddEntity();
	position_handler_->Add(handle, pos);
	shader_handler_->Add(handle, vs, fs);
	anim_handler_->Add(handle, anim_types);
	AddQuadShader(handle);
	return handle;
}

EntityID EntityHandler::MakePlayer(PositionHandler::Box const pos)
{
	ShaderHandler::VertexShader const vs = ShaderHandler::VertexShader::QUAD;
	ShaderHandler::FragmentShader const fs = ShaderHandler::FragmentShader::QUAD;
	unordered_set<AnimationHandler::AnimType> const anim_types{ 
				AnimationHandler::AnimType::PLAYERWALKUP,
				AnimationHandler::AnimType::PLAYERWALKDOWN,
				AnimationHandler::AnimType::PLAYERWALKLEFT,
				AnimationHandler::AnimType::PLAYERWALKRIGHT };

	auto handle = MakeCharacter(pos, vs, fs, anim_types);
	anim_handler_->SetActive(handle, AnimationHandler::AnimType::PLAYERWALKDOWN);
	event_handler_->Subscribe(handle,
		{ EventHandler::Event::KD_W, EventHandler::Event::KD_A,
		EventHandler::Event::KD_S, EventHandler::Event::KD_D });
	AddBehavior(handle, { EventHandler::Event::KD_W, EventHandler::Event::KU_W,
				EventHandler::Event::KD_A, EventHandler::Event::KU_A,
				EventHandler::Event::KD_S, EventHandler::Event::KU_S,
				EventHandler::Event::KD_D, EventHandler::Event::KU_D },
		{ Behavior::Controllable, Behavior::Walk });
	return handle;
}

EntityID EntityHandler::MakeRenderer()
{
	EntityID const handle = AddEntity();
	event_handler_->Subscribe(handle, { EventHandler::Event::KD_ESC });
	AddBehavior(handle, { EventHandler::Event::KD_ESC }, { Behavior::Exit });
	return handle;
}

EntityID EntityHandler::MakeCamera(PositionHandler::Box const ent_box)
{
	EntityID const handle = AddEntity();
	position_handler_->Add(handle, ent_box);
	renderer_->AddOrthoCamera(handle, ent_box.dims.x, ent_box.dims.y, ent_box.dir);
	return handle;
}

void EntityHandler::ActiveCam(EntityID const cam)
{
	renderer_->ActiveCam(cam);
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

