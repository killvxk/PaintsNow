// SnowyStream.h
// By PaintDream (paintdream@paintdream.com)
// 2015-1-8
//

#ifndef __SNOWYSTREAM_H__
#define __SNOWYSTREAM_H__

#include "../../Core/Interface/Interfaces.h"
#include <string>
#include "File.h"
#include "Zipper.h"
#include "ResourceBase.h"
#include "ResourceManager.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class SnowyStream : public IScript::Library {
		public:
			SnowyStream(Interfaces& interfaces, const TWrapper<IArchive*, IStreamBase&, size_t>& subArchiveCreator, const TWrapper<void, const String&>& errorHandler);
			virtual ~SnowyStream();
			virtual bool Initialize() override;
			virtual void Uninitialize() override;
			virtual void Reset();
			virtual Interfaces& GetInterfaces() const;
			virtual ResourceBase* CreateResource(const String& location, const String& extension, bool openExisting = true, IStreamBase* sourceStream = nullptr);
			virtual bool PersistResource(ResourceBase* resource, const String& extension = "");
			virtual bool RegisterResourceManager(Unique unique, ResourceManager* resourceManager);
			virtual bool RegisterResourceSerializer(Unique unique, const String& extension, ResourceSerializerBase* serializer);

		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			void RequestCreateResource(IScript::Request& request, String& expectedResType, String& path, bool createAlways);
			void RequestLoadExternalResourceData(IScript::Request& request, IScript::Delegate<ResourceBase> resource, String& externalPath);
			void RequestGetResourceInfo(IScript::Request& request, IScript::Delegate<ResourceBase> resource);
			void RequestPersistResource(IScript::Request& request, IScript::Delegate<ResourceBase> resource, String& extension);

			void RequestCreateZipper(IScript::Request& request, String& path);
			void RequestPostZipperData(IScript::Request& request, IScript::Delegate<Zipper> zipper, String& path, String& data);
			void RequestWriteZipper(IScript::Request& request, IScript::Delegate<File> file, IScript::Delegate<Zipper> zipper);

			void RequestCreateFile(IScript::Request& request, String& path, bool write);
			void RequestFileExists(IScript::Request& request, String& path);
			void RequestFlushFile(IScript::Request& request, IScript::Delegate<File> file);
			void RequestReadFile(IScript::Request& request, IScript::Delegate<File> file, int64_t length, IScript::Request::Ref callback);
			void RequestGetFileSize(IScript::Request& request, IScript::Delegate<File> file);
			void RequestGetFileLastModifiedTime(IScript::Request& request, IScript::Delegate<File> file);
			void RequestWriteFile(IScript::Request& request, IScript::Delegate<File> file, String& content, IScript::Request::Ref callback);
			void RequestCloseFile(IScript::Request& request, IScript::Delegate<File> file);
			void RequestSeekFile(IScript::Request& request, IScript::Delegate<File> file, String& type, int64_t offset);
			void RequestQueryFiles(IScript::Request& request, String& path);
			void RequestQuickTextResource(IScript::Request& request, String& path);

		public:
			void RegisterBuiltinPipelines();
			template <class T>
			bool RegisterResourceSerializer(const String& extension, T& device, ResourceSerializerBase* serializer) {
				Unique unique = UniqueType<T>::Get();
				if (!RegisterResourceSerializer(unique, extension, serializer)) {
					return false;
				}

				if (resourceManagers.count(unique) == 0) {
					ResourceManager* resourceManager = new DeviceResourceManager<T>(threadApi, &interfaces, device, errorHandler);
					RegisterResourceManager(unique, resourceManager);
					resourceManager->ReleaseObject();
				}

				return true;
			}

			static String GetReflectedExtension(Unique unique);

			template <class T>
			void RegisterReflectedSerializer(UniqueType<T> type, typename T::DriverType& device) {
				ResourceSerializerBase* serializer = new ResourceReflectedSerializer<T>();
				RegisterResourceSerializer(GetReflectedExtension(type.Get()), device, serializer);
				serializer->ReleaseObject();
			}

			template <class T>
			T* CreateReflectedResource(UniqueType<T> type, const String& location, bool openExisting = true, IStreamBase* sourceStream = nullptr) {
				return static_cast<T*>(CreateResource(location, GetReflectedExtension(type.Get()), openExisting, sourceStream));
			}

			void CreateBuiltinResources();

		protected:
			virtual void ScriptUninitialize(IScript::Request& request);
			void RegisterReflectedSerializers();
			bool FilterPath(const String& path);

		protected:
			Interfaces& interfaces;
			const TWrapper<void, const String&> errorHandler;
			unordered_map<String, std::pair<Unique, TShared<ResourceSerializerBase> > > resourceSerializers;
			std::map<Unique, TShared<ResourceManager> > resourceManagers;
			static String reflectedExtension;
			const TWrapper<IArchive*, IStreamBase&, size_t> subArchiveCreator;
		};
	}
}


#endif // __SNOWYSTREAM_H__
