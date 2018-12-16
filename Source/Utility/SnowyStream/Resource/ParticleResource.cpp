#include "ParticleResource.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

ParticleResource::ParticleResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : GraphicResourceBase(manager, uniqueID) {}

uint64_t ParticleResource::GetMemoryUsage() const {
	return 0;
}

bool ParticleResource::operator << (IStreamBase& stream) {
	return false;
}

bool ParticleResource::operator >> (IStreamBase& stream) {
	return false;
}

void ParticleResource::Attach(IRender& render) {
}

void ParticleResource::Detach(IRender& render) {

}

void ParticleResource::Upload(IRender& render) {

}

void ParticleResource::Download(IRender& render) {

}
