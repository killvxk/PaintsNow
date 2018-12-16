// EventListenerComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-16
//

#ifndef __EVENTLISTENERCOMPONENT_H__
#define __EVENTLISTENERCOMPONENT_H__

#include "../../../../Core/Template/TEvent.h"
#include "../../../BridgeSunset/BridgeSunset.h"
#include "../../Component.h"

namespace PaintsNow {
	namespace NsHeartVioliner {
		class Clock;
	}
	namespace NsMythForest {
		class EventListenerComponent : public Component, public QuickTask {
		public:
			EventListenerComponent();

			TEvent<Event&> EventNotifiers;
			void TickFrame(Engine& engine);

			void InstallTick(Engine& engine, TShared<NsHeartVioliner::Clock> clock);
			void UninstallTick(Engine& engine);

			enum {
				EVENTLISTENER_INSTALLED_TICK = COMPONENT_CUSTOM_BEGIN,
				EVENTLISTENER_BASE = COMPONENT_CUSTOM_BEGIN << 1,
				EVENTLISTENER_END = EVENTLISTENER_BASE << Event::EVENT_END,
				EVENTLISTENER_ALL = EVENTLISTENER_END - EVENTLISTENER_BASE,
				EVENTLISTENER_CUSTOM_BEGIN = EVENTLISTENER_BASE << Event::EVENT_END,
			};

			virtual void Initialize(Engine& engine, Entity* entity) override;
			virtual void Uninitialize(Engine& engine, Entity* entity) override;
			virtual void Execute(void* context) override;

			void SetCallback(IScript::Request& request, IScript::Request::Ref ref);
			IScript::Request::Ref GetCallback() const;

		protected:
			void Dispatch(Event& event);
			virtual void ScriptUninitialize(IScript::Request& request) override;
			IScript::Request::Ref callback;
			TShared<NsHeartVioliner::Clock> clock;
			Entity* rootEntity;
		};
	}
}

#endif // __EVENTLISTENERCOMPONENT_H__