// Controller.h
// By PaintDream
// 2016-3-22
//

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "ProxyStub.h"
#include "../../Core/Interface/IAsset.h"

namespace PaintsNow {
	namespace NsGalaxyWeaver {
		class Controller : public ProxyStub {
		public:
			Controller(IThread& threadApi, ITunnel& tunnel, const String& entry);
			virtual ~Controller();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		public:
			TWrapper<void, IScript::Request&, const IScript::Request::AutoWrapperBase&> CheckVersion;
			TWrapper<void, IScript::Request&, const IScript::Request::AutoWrapperBase&, bool, String&, String&, String&> PostResource;
			// TWrapper<void, IScript::Request&, const IScript::Request::AutoWrapperBase&, String&, ZCamera&> PostCamera;
		};
	}
}

#endif // __CONTROLLER_H__