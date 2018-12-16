// GalaxyWeaver
// By PaintDream
// 

#ifndef __GALAXYWEAVER_H__
#define __GALAXYWEAVER_H__

#include "../../Core/Interface/IScript.h"
#include "../../Core/Interface/ITunnel.h"
#include "Weaver.h"
#include "../MythForest/Entity.h"

namespace PaintsNow {
	namespace NsGalaxyWeaver {
		class GalaxyWeaver : public IScript::Library {
		public:
			GalaxyWeaver(IThread& threadApi, ITunnel& network, NsBridgeSunset::BridgeSunset& bridgeSunset, NsSnowyStream::SnowyStream& snowyStream, const TWrapper<void, const String&>& consoleWriter);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		public:
			void RequestCreateWeaver(IScript::Request& request, String& config);
			void RequestConnectEntity(IScript::Request& requst, IScript::Delegate<Weaver> weaver, IScript::Delegate<NsMythForest::Entity> scene);
			void RequestSetWeaverRpcCallback(IScript::Request& request, IScript::Delegate<Weaver> weaver, IScript::Request::Ref callback);
			void RequestSetWeaverConnectionCallback(IScript::Request& request, IScript::Delegate<Weaver> weaver, IScript::Request::Ref callback);
			void RequestStartWeaver(IScript::Request& request, IScript::Delegate<Weaver> weaver);
			void RequestStopWeaver(IScript::Request& request, IScript::Delegate<Weaver> weaver);
			void RequestCommitWeaverChanges(IScript::Request& request, IScript::Delegate<Weaver> weaver);

		protected:
			ITunnel& network;
			NsBridgeSunset::BridgeSunset& bridgeSunset;
			NsSnowyStream::SnowyStream& snowyStream;
			const TWrapper<void, const String&> consoleWriter;
		};
	}
}


#endif // __GALAXYWEAVER_H__