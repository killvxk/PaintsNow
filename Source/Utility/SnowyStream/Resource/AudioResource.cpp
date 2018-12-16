#include "AudioResource.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

AudioResource::AudioResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : DeviceResourceBase<IAudio>(manager, uniqueID) {
	Flag() |= RESOURCE_ONLINE;
}

uint64_t AudioResource::GetMemoryUsage() const {
	return 0;
}

void AudioResource::Download(IAudio& audio) {
}

void AudioResource::Upload(IAudio& audio) {
	// no operations ...
}

void AudioResource::Attach(IAudio& audio) {
}

void AudioResource::Detach(IAudio& audio) {
}

IAudio::Buffer* AudioResource::GetAudioBuffer() {
	return audioBuffer;
}