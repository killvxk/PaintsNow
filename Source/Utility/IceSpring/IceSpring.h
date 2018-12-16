// IceSpring.h
// By PaintDream (paintdream@paintdream.com)
// 2015-4-13
//

#ifndef __ICESPRING_H__
#define __ICESPRING_H__

#include "../../Core/Interface/IScript.h"
#include "../../Core/Interface/IFrame.h"
#include "../HeartVioliner/Queue.h"
#include "../BridgeSunset/BridgeSunset.h"

namespace PaintsNow {
	namespace NsIceSpring {
		class Dispatcher;
		class IceSpring : public IScript::Library {
		public:
			IceSpring(IThread& thread, NsBridgeSunset::BridgeSunset& bridge);

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			void RequestCreateDispatch(IScript::Request& request, String& type, IScript::Request::Ref handler);
			void RequestPost(IScript::Request& request, IScript::Request::Ref ref, bool realTimeTask);

			void OnMouse(IScript::Request& request, const IFrame::EventMouse& mouse);
			void OnKeyboard(IScript::Request& request, const IFrame::EventKeyboard& keyboard);
			void OnResize(IScript::Request& request, const ZInt2& newSize);
			void Remove(Dispatcher* dispatcher);

		private:
			void ForwardMessage(IScript::Request* request, Dispatcher* disp, bool backTask);

		private:
			std::set<Dispatcher*> mouseRoutines;
			std::set<Dispatcher*> keyboardRoutines;
			std::set<Dispatcher*> resizeRoutines;
			std::set<Dispatcher*> postRoutines;
			NsBridgeSunset::BridgeSunset& bridgeSunset;
		};
	}
}

#endif // __ICESPRING_H__