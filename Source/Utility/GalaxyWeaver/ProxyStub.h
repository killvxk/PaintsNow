// ProxyStub.h
// By PaintDream
// 2016-3-22
//

#ifndef __PROXYSTUB_H__
#define __PROXYSTUB_H__

#include "../../Core/Interface/IScript.h"
#include "../../Core/Interface/IThread.h"
#include "../../Core/Interface/ITunnel.h"
#include "../../Core/Object/Tiny.h"
#include "../../Core/Common/ZRemoteProxy.h"

namespace PaintsNow {
	namespace NsGalaxyWeaver {
		class ProxyStub : public SharedTiny {
		public:
			ProxyStub(IThread& thread, ITunnel& tunnel, const String& entry, const TWrapper<void, IScript::Request&, bool, ZRemoteProxy::STATUS, const String&>& statusHandler = TWrapper<void, IScript::Request&, bool, ZRemoteProxy::STATUS, const String&>());

		protected:
			IScript::Object* StaticCreate(const String& entry);
			ZRemoteProxy remoteProxy;
		};
	}
}

#endif // __PROXYSTUB_H__