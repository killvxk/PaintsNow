#include "VolumeResource.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

VolumeResource::VolumeResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : GraphicResourceBase(manager, uniqueID) {}

uint64_t VolumeResource::GetMemoryUsage() const {
	return 0;
}

bool VolumeResource::operator << (IStreamBase& stream) {
	return false;
}

bool VolumeResource::operator >> (IStreamBase& stream) {
	return false;
}

void VolumeResource::Attach(IRender& render) {
}

void VolumeResource::Detach(IRender& render) {

}

void VolumeResource::Upload(IRender& render) {

}

void VolumeResource::Download(IRender& render) {

}
