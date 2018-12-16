#include "LeavesExporterPCH.h"
#include "Service.h"
#include "../../../../../Source/Driver/Tunnel/WinPipe/ZWinPipe.h"
#include "../../../../../Source/Driver/Thread/Pthread/ZThreadPthread.h"

namespace PaintsNow {
	namespace NsGalaxyWeaver {
		static ZThreadPthread uniqueThreadApi;
		static UniqueAllocator allocator(uniqueThreadApi);
		static ZRemoteFactory remoteFactory;
		static ZWinPipe winPipe(1, 4096);
		static ProviderFactory localFactory(uniqueThreadApi, winPipe);

		class UnrealResourceManager : public NsSnowyStream::ResourceManager {
		public:
			UnrealResourceManager(IThread& threadApi) : ResourceManager(threadApi, nullptr, TWrapper<void, const String&>()) {}
			virtual Unique GetDeviceUnique() const {
				return UniqueType<UObject>::Get();
			}

			virtual void InvokeAttach(NsSnowyStream::ResourceBase* resource) override {}
			virtual void InvokeDetach(NsSnowyStream::ResourceBase* resource) override {}
			virtual void InvokeUpload(NsSnowyStream::ResourceBase* resource) override {}
			virtual void InvokeDownload(NsSnowyStream::ResourceBase* resource) override {}
		};

		void Service::StatusHandler(IScript::Request& request, bool isServer, ZRemoteProxy::STATUS status, const String& message) {
			if (status == ZRemoteProxy::CONNECTED) {
				// Check Version
				IScript::Request::Ref global = request.Load("Global", "Initialize");
				request.QueryInterface(Wrap(this, &Service::OnInitializeQuery), remoteFactory, global);
			}
		}

		void Service::Initialize(ISceneExplorer* se) {
			sceneExp = se;
			SetGlobalUniqueAllocator(&allocator);
			resourceManager = std::make_unique<UnrealResourceManager>(uniqueThreadApi);
			remoteProxy = std::make_unique<ZRemoteProxy>(uniqueThreadApi, winPipe, localFactory, "", Wrap(this, &Service::StatusHandler));
			// connecting ...
			remoteProxy->Run();
			// Reconnect("LeavesWingScenePoster");
		}

		void Service::Reconnect(const String& port) {
			mainRequest.reset(remoteProxy->NewRequest(port));
		}

		void Service::Deinitialize() {
			// order assurance 
			mainRequest.release();
			controller.release();
			remoteProxy.release();
		}

		void Service::OnInitializeQuery(IScript::Request& request, IReflectObject& inter, const IScript::Request::Ref& ref) {
			assert(&inter == &remoteFactory);
			remoteFactory.CreateObject(request, request.Adapt(Wrap(this, &Service::OnCreateWeaverObject)), "Weaver");
		}

		void Service::OnCreateWeaverObject(IScript::Request& request, IScript::Request::Ref instance) {
			controller.reset(new Controller(uniqueThreadApi, winPipe, ""));
			request.QueryInterface(Wrap(this, &Service::OnWeaverObjectQuery), *controller, instance);
		}

		void Service::OnWeaverObjectQuery(IScript::Request& request, IReflectObject& inter, const IScript::Request::Ref& ref) {
			// instance: can be used to make rpc calls to remotely created weaver instance
			// Now we just check version
			request.Push();
			controller->CheckVersion(request, request.Adapt(Wrap(this, &Service::OnCheckVersion)));
			request.Pop();
		}

		void Service::OnCheckVersion(IScript::Request& request, String& mainVersion, String& subVersion, String& buildVersion) {
			String str = "OnCheckVersion(): ";
			str += mainVersion + " " + subVersion + " " + buildVersion;
			sceneExp->WriteLog(ISceneExplorer::LOG_TEXT, str.c_str());
		}

		NsSnowyStream::ResourceManager& Service::GetResourceManager() const {
			assert(resourceManager.get() != nullptr);
			return *resourceManager.get();
		}

		IScript::Request& Service::GetMainRequest() const {
			assert(mainRequest.get() != nullptr);
			return *mainRequest.get();
		}
	}
}