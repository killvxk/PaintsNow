// TextResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-11
//

#ifndef __TEXTRESOURCE_H__
#define __TEXTRESOURCE_H__

#include "../ResourceBase.h"
#include "../../../Core/Interface/IArchive.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class TextResource : public DeviceResourceBase<IArchive> {
		public:
			TextResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual ~TextResource();
			virtual uint64_t GetMemoryUsage() const;
			virtual bool LoadExternalResource(IStreamBase& streamBase, size_t length);

			// re-pure-virtualize these interfaces
			virtual void Upload(IArchive& device);
			virtual void Download(IArchive& device);
			virtual void Attach(IArchive& device);
			virtual void Detach(IArchive& device);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			String text;
		};
	}
}


#endif // __TEXTRESOURCE_H__