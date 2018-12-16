#include "ResourceBase.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

ResourceBase::ResourceBase(ResourceManager& manager, const ResourceManager::UniqueLocation& id) : SharedTiny(Tiny::TINY_UNIQUE | Tiny::TINY_READONLY | Tiny::TINY_ACTIVATED), resourceManager(manager), uniqueLocation(id) {}

std::pair<uint16_t, uint16_t> ResourceBase::GetProgress() const {
	return std::make_pair(0, 1);
}

ResourceManager& ResourceBase::GetResourceManager() const {
	return resourceManager;
}

void ResourceBase::ReleaseObject() {
	if (referCount.load(std::memory_order_acquire) == 1) {
		// no references exist, remove this from resource manager
		resourceManager.Remove(this);
	}

	SharedTiny::ReleaseObject();
}

bool ResourceBase::LoadExternalResource(IStreamBase& streamBase, size_t length) {
	return false; // by default no external resource supported.
}

void ResourceBase::GetDependencies(std::vector<TShared<ResourceBase> >& deps) const {
	// do nothing by default
}

TObject<IReflect>& ResourceBase::operator () (IReflect& reflect) {
	ReflectClass(ResourceBase)[Interface(Tiny)];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(referCount)[MetaRuntime()];
	}

	return *this;
}

const String& ResourceBase::GetLocation() const {
	return uniqueLocation;
}

void ResourceBase::SetLocation(const ResourceManager::UniqueLocation& location) {
	uniqueLocation = location;
}