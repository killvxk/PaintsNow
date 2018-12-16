#include "EventListenerComponentModule.h"
#include "../../Engine.h"
#include "../../../HeartVioliner/Clock.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

#define SET_EVENT_NAME(f) \
	eventNames[(int)f] = #f;

EventListenerComponentModule::EventListenerComponentModule(Engine& engine) : ModuleImpl<EventListenerComponent>(engine) {
	eventNames.resize(Event::EVENT_END);

	SET_EVENT_NAME(Event::EVENT_TICK);
	SET_EVENT_NAME(Event::EVENT_FRAME);
	SET_EVENT_NAME(Event::EVENT_ATTACH_COMPONENT);
	SET_EVENT_NAME(Event::EVENT_DETACH_COMPONENT);
	SET_EVENT_NAME(Event::EVENT_ENTITY_ACTIVATE);
	SET_EVENT_NAME(Event::EVENT_ENTITY_DEACTIVATE);
}

TObject<IReflect>& EventListenerComponentModule::operator () (IReflect& reflect) {
	ReflectClass(EventListenerComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestGetEventName)[ScriptMethod = "GetEventName"];
		ReflectMethod(RequestBindEventTick)[ScriptMethod = "BindEventTick"];
		ReflectMethod(RequestBindEventFrame)[ScriptMethod = "BindEventFrame"];
		ReflectMethod(RequestFilterEvent)[ScriptMethod = "FilterEvent"];
	}

	return *this;
}

void EventListenerComponentModule::RequestGetEventName(IScript::Request& request, int32_t id) {
	METHOD_ENTRY(EventListenerComponentModule::RequestGetEventName);
	CHECK_REFERENCES_NONE();

	if (id >= 0 && id < (int32_t)eventNames.size()) {
		request.DoLock();
		request << eventNames[id];
		request.UnLock();
	}
}

void EventListenerComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(EventListenerComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	TShared<EventListenerComponent> eventListenerComponent = TShared<EventListenerComponent>::Make();
	request.DoLock();
	request << delegate(eventListenerComponent());
	request.UnLock();
}

void EventListenerComponentModule::RequestBindEventTick(IScript::Request& request, IScript::Delegate<EventListenerComponent> eventListenerComponent, IScript::Delegate<NsHeartVioliner::Clock> clock) {
	METHOD_ENTRY(EventListenerComponentModule::RequestBindEventFrame);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(eventListenerComponent);

	if (clock) {
		eventListenerComponent->InstallTick(engine, clock.Get());
	} else {
		eventListenerComponent->UninstallTick(engine);
	}
}

void EventListenerComponentModule::RequestFilterEvent(IScript::Request& request, IScript::Delegate<EventListenerComponent> eventListenerComponent, uint32_t mask) {
	METHOD_ENTRY(EventListenerComponentModule::RequestBindEventFrame);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(eventListenerComponent);

	uint32_t flag, old;
	do {
		flag = old = eventListenerComponent->Flag().load(std::memory_order_acquire);
		flag = (flag & ~(EventListenerComponent::EVENTLISTENER_ALL)) | (mask * EventListenerComponent::EVENTLISTENER_BASE & EventListenerComponent::EVENTLISTENER_ALL);
	} while (eventListenerComponent->Flag().compare_exchange_weak(old, flag, std::memory_order_release));
}

void EventListenerComponentModule::RequestBindEventFrame(IScript::Request& request, IScript::Delegate<EventListenerComponent> eventListenerComponent, bool add) {
	METHOD_ENTRY(EventListenerComponentModule::RequestBindEventFrame);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(eventListenerComponent);

	DoLock();
	if (add) {
		frameTickedListeners.insert(eventListenerComponent.Get());
	} else {
		frameTickedListeners.erase(eventListenerComponent.Get());
	}
	UnLock();
}

void EventListenerComponentModule::TickFrame() {
	DoLock();
	std::vector<TShared<EventListenerComponent> > nextTickedListeners;
	nextTickedListeners.reserve(frameTickedListeners.size());

	for (std::set<TShared<EventListenerComponent> >::iterator it = frameTickedListeners.begin(); it != frameTickedListeners.end(); ) {
		EventListenerComponent* component = (*it)();
		if (component->GetReferCount() != 1) {
			nextTickedListeners.push_back(component);
			++it;
		} else {
			std::set<TShared<EventListenerComponent> >::iterator ip = it++;	
			frameTickedListeners.erase(ip);
		}
	}
	UnLock();

	for (size_t j = 0; j < nextTickedListeners.size(); j++) {
		nextTickedListeners[j]->TickFrame(engine);
	}
}