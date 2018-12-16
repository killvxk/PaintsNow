// Dispatcher.h
// By PaintDream (paintdream@paintdream.com)
// 2015-6-23
//

#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include "../../Core/Interface/IScript.h"
#include "../../Core/Object/Tiny.h"

namespace PaintsNow {
	namespace NsIceSpring {
		class IceSpring;
		class Dispatcher : public SharedTiny {
		public:
			Dispatcher(IceSpring& h, IScript::Request::Ref ref);

			const IScript::Request::Ref& GetCallback() const;
			virtual void ScriptUninitialize(IScript::Request& request);
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			void Invoke(IScript::Request& request);

		private:
			IceSpring& host;
			IScript::Request::Ref callback;
		};
	}
}


#endif // __DISPATCHER_H__