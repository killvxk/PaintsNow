// ResourceBase.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __RESOURCEBASE_H__
#define __RESOURCEBASE_H__

#include "../../Core/Object/Tiny.h"
#include "ResourceManager.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class ResourceBase : public SharedTiny {
		public:
			enum {
				RESOURCE_UPLOADED = TINY_CUSTOM_BEGIN << 0,
				RESOURCE_DOWNLOADED = TINY_CUSTOM_BEGIN << 1,
				RESOURCE_ONLINE = TINY_CUSTOM_BEGIN << 2,
				RESOURCE_RESERVE_LOCALDATA = TINY_CUSTOM_BEGIN << 3,
				RESOURCE_ETERNAL = TINY_CUSTOM_BEGIN << 4,
				RESOURCE_CUSTOM_BEGIN = TINY_CUSTOM_BEGIN << 5
			};
			typedef Void DriverType;
			ResourceBase(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueLocation);
			ResourceManager& GetResourceManager() const;
			const ResourceManager::UniqueLocation& GetLocation() const;
			void SetLocation(const ResourceManager::UniqueLocation& location);
			virtual uint64_t GetMemoryUsage() const = 0;
			virtual Unique GetDeviceUnique() const = 0;
			virtual bool LoadExternalResource(IStreamBase& streamBase, size_t length);
			virtual void GetDependencies(std::vector<TShared<ResourceBase> >& deps) const;
			
			inline bool IsPrepared() const {
				std::pair<uint16_t, uint16_t> progress = GetProgress();
				return progress.first == progress.second;
			}

			virtual std::pair<uint16_t, uint16_t> GetProgress() const;
			virtual void ReleaseObject() override;
			// override them derived from IReflectObject to change behaviors on serialization & deserialization
			// virtual bool operator >> (IStreamBase& stream) const;
			// virtual bool operator << (IStreamBase& stream);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			ResourceManager::UniqueLocation uniqueLocation;
			ResourceManager& resourceManager;
		};

		template <class T>
		class DeviceResourceBase : public ResourceBase {
		public:
			typedef T DriverType;
			DeviceResourceBase(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueLocation) : ResourceBase(manager, uniqueLocation) {}

			virtual void Download(T& device) = 0;
			virtual void Upload(T& device) = 0;
			virtual void Attach(T& device) = 0;
			virtual void Detach(T& device) = 0;

			virtual Unique GetDeviceUnique() const {
				return UniqueType<T>::Get();
			}
		};
	}
}

#endif // __RESOURCEBASE_H__
