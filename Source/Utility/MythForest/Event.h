// Event.h
// By PaintDream (paintdream@paintdream.com)
// 2015-9-12
//

#ifndef __EVENT_H__
#define __EVENT_H__

#include "../../Core/Object/Tiny.h"
#include "Engine.h"

namespace PaintsNow {
	namespace NsMythForest {
		class Event {
		public:
			enum EVENT_ID {
				EVENT_TICK,
				EVENT_FRAME,
				// TACH events
				EVENT_ATTACH_COMPONENT,
				EVENT_DETACH_COMPONENT,
				EVENT_ENTITY_ACTIVATE,
				EVENT_ENTITY_DEACTIVATE,
				EVENT_CUSTOM,
				EVENT_END
			};

			Event(Engine& engine, EVENT_ID id, TShared<SharedTiny> sender, TShared<SharedTiny> detail = nullptr);

			Engine& engine;
			EVENT_ID eventID;
			TShared<SharedTiny> eventSender;
			TShared<SharedTiny> eventDetail;
		};
	}
}


#endif // __EVENT_H__