#include "ResourceBase.h"
#include "ResourceManager.h"
#include "../../Core/Interface/IArchive.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

ResourceManager::ResourceManager(IThread& threadApi, Interfaces* inters, const TWrapper<void, const String&>& err) : ISyncObject(threadApi), interfaces(inters), errorHandler(err) {
}

ResourceManager::~ResourceManager() {
	assert(resourceMap.empty());
}

void ResourceManager::Report(const String& err) {
	if (errorHandler)
		errorHandler(err);
}

Interfaces* ResourceManager::GetInterfaces() const {
	return interfaces;
}

void ResourceManager::RemoveAll() {
	DoLock();

	// remove all eternal resources
	unordered_map<UniqueLocation, ResourceBase*> temp;
	std::swap(temp, resourceMap);
	for (unordered_map<UniqueLocation, ResourceBase*>::const_iterator it = temp.begin(); it != temp.end(); ++it) {
		ResourceBase* resource = (*it).second;
		if (resource->Flag() & ResourceBase::RESOURCE_ETERNAL) {
			resource->ReleaseObject();
		}
	}
	UnLock();
}

ResourceBase* ResourceManager::Insert(const UniqueLocation& id, ResourceBase* resource) {
	assert(resource != nullptr);
	DoLock();
	assert(resourceMap.count(id) == 0);
	resourceMap[id] = resource;
	InvokeAttach(resource);
	UnLock();

	return resource;
}

ResourceBase* ResourceManager::LoadExist(const UniqueLocation& id) {
	DoLock();
	unordered_map<UniqueLocation, ResourceBase*>::iterator p = resourceMap.find(id);
	ResourceBase* pointer = nullptr;
	if (p != resourceMap.end()) {
		pointer = (*p).second;
		assert(pointer != nullptr);
		pointer->ReferenceObject();
	}

	UnLock();
	return pointer;
}

void ResourceManager::Remove(ResourceBase* resource) {
	assert(resource != nullptr);

	DoLock();
	// Double check;
	assert(resource->GetReferCount() == 1);
	resourceMap.erase(resource->GetLocation());
	InvokeDetach(resource);
	UnLock();
}

inline ResourceManager::UniqueLocation PathToUniqueID(const String& path) {
	return path;
}

ResourceBase* ResourceSerializerBase::DeserializeFromArchive(ResourceManager& manager, IArchive& archive, const String& path, IFilterBase& protocol, bool openExisting) {
	assert(manager.GetDeviceUnique() == GetDeviceUnique());
	if (manager.GetDeviceUnique() != GetDeviceUnique())
		return nullptr;
		
	ResourceBase* resource = manager.LoadExist(PathToUniqueID(path));
	if (resource != nullptr) {
		return resource;
	}

	size_t length;
	if (openExisting) {
		IStreamBase* stream = archive.Open(path, false, length);
		if (stream != nullptr) {
			resource = Deserialize(manager, path, protocol, stream);
			if (resource != nullptr) {
				if (!(resource->Flag() & ResourceBase::RESOURCE_ONLINE)) {
					stream->ReleaseObject();
				}
			} else {
				stream->ReleaseObject();
			}
		}
	} else {
		resource = Deserialize(manager, path, protocol, nullptr);
	}

	return resource;
}

bool ResourceSerializerBase::SerializeToArchive(ResourceBase* resource, IArchive& archive, IFilterBase& protocol, const String& path) {
	assert(resource != nullptr);

	size_t length;
	IStreamBase* stream = archive.Open(path, true, length);
	if (stream != nullptr) {
		bool result = Serialize(resource, protocol, *stream);
		stream->ReleaseObject();

		if (!result) {
			archive.Delete(path);
		}

		return result;
	}

	return false;
}

ResourceSerializerBase::~ResourceSerializerBase() {}
