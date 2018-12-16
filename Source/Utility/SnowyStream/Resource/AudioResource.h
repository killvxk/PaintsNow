// AudioResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __AUDIO_RESOURCE_H__
#define __AUDIO_RESOURCE_H__

#include "../ResourceBase.h"
#include "../../../Core/Interface/IAudio.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class AudioResource : public DeviceResourceBase<IAudio> {
		public:
			AudioResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual uint64_t GetMemoryUsage() const;
			virtual void Download(IAudio& device);
			virtual void Upload(IAudio& device);
			virtual void Attach(IAudio& device);
			virtual void Detach(IAudio& device);

			IAudio::Buffer* GetAudioBuffer();

		protected:
			IAudio::Buffer* audioBuffer;
		};
	}
}

#endif // __AUDIO_RESOURCE_H__