#include "EventListenerComponent.h"
#include "../../Entity.h"
#include "../../../HeartVioliner/Clock.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsHeartVioliner;

EventListenerComponent::EventListenerComponent() : rootEntity(nullptr) {}

void EventListenerComponent::Execute(void* context) {
	if (rootEntity != nullptr) {
		Event event(*reinterpret_cast<Engine*>(context), Event::EVENT_TICK, rootEntity);
		rootEntity->PostEvent(event);
	}
}

void EventListenerComponent::InstallTick(Engine& engine, TShared<Clock> c) {
	if (!(Flag() & EVENTLISTENER_INSTALLED_TICK)) {
		Flag() |= EVENTLISTENER_INSTALLED_TICK;
		clock = c;
		clock->AddTicker(this, &engine);
	}
}

void EventListenerComponent::UninstallTick(Engine& engine) {
	if (Flag() & EVENTLISTENER_INSTALLED_TICK) {
		clock->RemoveTicker(this);
		Flag() &= ~EVENTLISTENER_INSTALLED_TICK;
	}
}

void EventListenerComponent::ScriptUninitialize(IScript::Request& request) {
	if (callback) {
		request.DoLock();
		request.Dereference(callback);
		request.UnLock();
	}

	Component::ScriptUninitialize(request);
}

void EventListenerComponent::SetCallback(IScript::Request& request, IScript::Request::Ref ref) {
	if (callback) {
		request.Dereference(callback);
	}

	callback = request.MakeReference(ref);

	if (callback) {
		EventNotifiers += Wrap(this, &EventListenerComponent::Dispatch);
	}
}

IScript::Request::Ref EventListenerComponent::GetCallback() const {
	return callback;
}

void EventListenerComponent::Dispatch(Event& event) {
	if (Flag() & (EventListenerComponent::EVENTLISTENER_BASE << event.eventID)) {
		if (callback && event.eventID != Event::EVENT_FRAME) {
			event.engine.bridgeSunset.Dispatch(NsBridgeSunset::WrapTaskScript(callback, delegate(this), (int)event.eventID));
		}
	}
}

void EventListenerComponent::Initialize(Engine& engine, Entity* entity) {
	Component::Initialize(engine, entity);
	// enables all event slots ...
	assert(rootEntity == nullptr);
	rootEntity = entity;
}

void EventListenerComponent::Uninitialize(Engine& engine, Entity* entity) {
	assert(rootEntity != nullptr);
	rootEntity = entity;
	Component::Uninitialize(engine, entity);
}

void EventListenerComponent::TickFrame(Engine& engine) {
	if (rootEntity != nullptr) {
		Event event(engine, Event::EVENT_FRAME, rootEntity);
		rootEntity->PostEvent(event);
	}
}