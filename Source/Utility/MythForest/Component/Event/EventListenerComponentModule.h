// EventListenerComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-16
//

#ifndef __EVENTLISTENERCOMPONENTMODULE_H__
#define __EVENTLISTENERCOMPONENTMODULE_H__

#include "EventListenerComponent.h"
#include "../../../BridgeSunset/BridgeSunset.h"

namespace PaintsNow {
	namespace NsHeartVioliner {
		class Clock;
	}
	namespace NsMythForest {
		class EventListenerComponent;
		class EventListenerComponentModule : public ModuleImpl<EventListenerComponent> {
		public:
			EventListenerComponentModule(Engine& engine);
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual void TickFrame() override;

			void RequestCreate(IScript::Request& request);
			void RequestGetEventName(IScript::Request& request, int32_t id);
			void RequestBindEventTick(IScript::Request& request, IScript::Delegate<EventListenerComponent> eventListenerComponent, IScript::Delegate<NsHeartVioliner::Clock> clock);
			void RequestBindEventFrame(IScript::Request& request, IScript::Delegate<EventListenerComponent> eventListenerComponent, bool enable);
			void RequestFilterEvent(IScript::Request& request, IScript::Delegate<EventListenerComponent> eventListnerComponent, uint32_t idMask);

		protected:
			std::vector<String> eventNames;
			std::set<TShared<EventListenerComponent> > frameTickedListeners;
		};
	}
}

#endif // __EVENTLISTENERCOMPONENTMODULE_H__