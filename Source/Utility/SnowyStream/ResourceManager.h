// ResourceManager.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-19
//

#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "../../Core/Interface/Interfaces.h"
#include "../../Core/Interface/IFilterBase.h"
#include "../../Core/Template/TMap.h"
#include "../../Core/Object/Tiny.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class ResourceBase;
		template <class T>
		class DeviceResourceBase;
		class ResourceManager : public SharedTiny, public ISyncObject {
		public:
			ResourceManager(IThread& threadApi, Interfaces* interfaces, const TWrapper<void, const String&>& errorHandler);
			virtual ~ResourceManager();
			typedef String UniqueLocation;
			virtual ResourceBase* Insert(const UniqueLocation& uniqueLocation, ResourceBase* resource);
			void Remove(ResourceBase* resource);
			void RemoveAll();
			virtual Unique GetDeviceUnique() const = 0;
			Interfaces* GetInterfaces() const;
			void Report(const String& err);

			ResourceBase* LoadExist(const UniqueLocation& uniqueLocation);

		public:
			virtual void InvokeAttach(ResourceBase* resource) = 0;
			virtual void InvokeDetach(ResourceBase* resource) = 0;
			virtual void InvokeUpload(ResourceBase* resource) = 0;
			virtual void InvokeDownload(ResourceBase* resource) = 0;

		private:
			unordered_map<UniqueLocation, ResourceBase*> resourceMap;
			Interfaces* interfaces;
			TWrapper<void, const String&> errorHandler;
		};

		template <class T>
		class DeviceResourceManager : public ResourceManager {
		public:
			DeviceResourceManager(IThread& threadApi, Interfaces* inters, T& dev, const TWrapper<void, const String&>& errorHandler) : ResourceManager(threadApi, inters, errorHandler), device(dev) {}
			virtual Unique GetDeviceUnique() const {
				return UniqueType<T>::Get();
			}

			virtual void InvokeAttach(ResourceBase* resource) override {
				assert(resource != nullptr);
				DeviceResourceBase<T>* typedResource = static_cast<DeviceResourceBase<T>*>(resource);
				typedResource->Attach(device);
			}

			virtual void InvokeDetach(ResourceBase* resource) override {
				assert(resource != nullptr);
				DeviceResourceBase<T>* typedResource = static_cast<DeviceResourceBase<T>*>(resource);
				typedResource->Detach(device);
			}

			virtual void InvokeUpload(ResourceBase* resource) override {
				assert(resource != nullptr);
				DeviceResourceBase<T>* typedResource = static_cast<DeviceResourceBase<T>*>(resource);
				typedResource->Upload(device);
			}

			virtual void InvokeDownload(ResourceBase* resource) override {
				assert(resource != nullptr);
				DeviceResourceBase<T>* typedResource = static_cast<DeviceResourceBase<T>*>(resource);
				typedResource->Download(device);
			}

		protected:
			T& device;
		};

		class ResourceSerializerBase : public SharedTiny {
		public:
			virtual ~ResourceSerializerBase();
			ResourceBase* DeserializeFromArchive(ResourceManager& manager, IArchive& archive, const String& path,  IFilterBase& protocol, bool openExisting);
			bool SerializeToArchive(ResourceBase* resource, IArchive& archive, IFilterBase& protocol, const String& path);

			virtual ResourceBase* Deserialize(ResourceManager& manager, const ResourceManager::UniqueLocation& id, IFilterBase& protocol, IStreamBase* stream) = 0;
			virtual bool Serialize(ResourceBase* res, IFilterBase& protocol, IStreamBase& stream) = 0;
			virtual Unique GetDeviceUnique() const = 0;
		};

		template <class T>
		class ResourceReflectedSerializer : public ResourceSerializerBase {
		protected:
			virtual Unique GetDeviceUnique() const {
				return UniqueType<typename T::DriverType>::Get();
			}

			virtual ResourceBase* Deserialize(ResourceManager& manager, const ResourceManager::UniqueLocation& id, IFilterBase& protocol, IStreamBase* stream) {
				T* object = new T(manager, id);

				if (stream != nullptr) {
					IStreamBase* filter = protocol.CreateFilter(*stream);
					assert(filter != nullptr);
					if (!(*filter >> *object)) {
						object->ReleaseObject();
						object = nullptr;
					} else {
						manager.Insert(id, object);
						manager.InvokeUpload(object); // upload because data is prepared.
					}

					filter->ReleaseObject();
				} else {
					manager.Insert(id, object);
				}

				return object;
			}

			virtual bool Serialize(ResourceBase* object, IFilterBase& protocol, IStreamBase& stream) {
				IStreamBase* filter = protocol.CreateFilter(stream);
				assert(filter != nullptr);
				bool state = *filter << *object;
				filter->ReleaseObject();

				return state;
			}
		};
	}
}


#endif // __RESOURCEMANAGER_H__