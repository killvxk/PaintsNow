// Weaver.h
// By PaintDream
// 2016-3-22
//

#ifndef __WEAVER_H__
#define __WEAVER_H__

#include "Controller.h"
#include "../BridgeSunset/BridgeSunset.h"
#include "../SnowyStream/SnowyStream.h"

namespace PaintsNow {
	namespace NsGalaxyWeaver {
		class Weaver : public ProxyStub {
		public:
			Weaver(NsBridgeSunset::BridgeSunset& bridgeSunset, NsSnowyStream::SnowyStream& snowyStream, ITunnel& tunnel, const String& entry, const TWrapper<void, const String&>& cw);
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual void ScriptUninitialize(IScript::Request& request) override;
			void SetRpcCallback(IScript::Request& request, const IScript::Request::Ref& ref);
			void SetConnectionCallback(IScript::Request& request, const IScript::Request::Ref& ref);

		public:
			void OnConnectionStatus(IScript::Request& request, bool state, ZRemoteProxy::STATUS status, const String& message);
			// Local controls
			void Start();
			void Stop();

		protected:
			NsBridgeSunset::BridgeSunset& bridgeSunset;
			NsSnowyStream::SnowyStream& snowyStream;
			IScript::Request::Ref rpcCallback;
			IScript::Request::Ref connectionCallback;

		protected:
			void FilterRpcCall(const String& operation, const String& message);

		protected:
			// Remote routines
			void RpcCheckVersion(IScript::Request& request);
			void RpcInitialize(IScript::Request& request, String& clientVersion);
			void RpcUninitialize(IScript::Request& request);

			void RpcDebugPrint(IScript::Request& request, String& text);
			void RpcPostResource(IScript::Request& request, bool onfly, String& location, String& extension, String& resourceData);
			void RpcPostEntity(IScript::Request& request, bool onfly, String& entityData);
			void RpcCleanupFlyingResources(IScript::Request& request, String& extension);
			void RpcUpdateView(IScript::Request& request, String& cameraSettings);
			void RpcUpdateEntity(IScript::Request& request, String& guid, String& entityData);

		protected:
			std::vector<NsSnowyStream::ResourceBase*> flyingResources;
			const TWrapper<void, const String&>& consoleWriter;
			bool started;
		};
	}
}

#endif // __WEAVER_H__