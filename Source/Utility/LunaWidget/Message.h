// Message.h -- UI Message
// By PaintDream (paintdream@paintdream.com)
// 2015-1-20
//

#ifndef __MESSAGE_UI_H__
#define __MESSAGE_UI_H__

#include "../../Core/Template/TEvent.h"

namespace PaintsNow {
	namespace NsLunaWidget {
		class RequestState {
		public:
			RequestState();
			bool IsHandled() const;
			void Complete();

		private:
			bool handled;
		};

		typedef TEvent<RequestState&, bool> EventMouseMove;
		typedef TEvent<RequestState&, bool> EventMouseClick;
		typedef TEvent<RequestState&, bool> MessageShow;
		typedef TEvent<RequestState&, bool> MessageHighlight;
		typedef TEvent<RequestState&, int64_t> MessageTimer;
		typedef TEvent<RequestState&, bool> MessageLayout;
		typedef TEvent<RequestState&> MessageDestroy;
	}
}

#endif // __MESSAGE_UI_H__