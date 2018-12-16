// TapeResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-10
//

#ifndef __TAPE_RESOURCE_H__
#define __TAPE_RESOURCE_H__

#include "../ResourceBase.h"
#include "../../../Core/Interface/IArchive.h"
#include "../../../Core/Common/ZPacket.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class TapeResource : public DeviceResourceBase<IArchive> {
		public:
			TapeResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual ~TapeResource();

			virtual uint64_t GetMemoryUsage() const;
			virtual void Download(IArchive& device);
			virtual void Upload(IArchive& device);
			virtual void Attach(IArchive& device);
			virtual void Detach(IArchive& device);

			ZPacket* GetPacket();

		protected:
			void Close();
			ZPacket* packetProvider;
			IStreamBase* packetStream;
			String streamPath;
		};
	}
}

#endif // __TAPE_RESOURCE_H__