// VolumeResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __VOLUME_RESOURCE_H__
#define __VOLUME_RESOURCE_H__

#include "GraphicResourceBase.h"
#include "../../../Core/Interface/IAsset.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class VolumeResource : public GraphicResourceBase {
		public:
			VolumeResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual uint64_t GetMemoryUsage() const;
			virtual bool operator << (IStreamBase& stream);
			virtual bool operator >> (IStreamBase& stream);
			virtual void Upload(IRender& render);
			virtual void Download(IRender& render);
			virtual void Attach(IRender& render);
			virtual void Detach(IRender& render);
		};
	}
}

#endif // __VOLUME_RESOURCE_H__