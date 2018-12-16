// GraphicResourceBase.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-11
//

#ifndef __GRAPHICRESOURCEBASE_H__
#define __GRAPHICRESOURCEBASE_H__

#include "../ResourceBase.h"
#include "../../../Core/Interface/IRender.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class GraphicResourceBase : public DeviceResourceBase<IRender> {
		public:
			GraphicResourceBase(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual ~GraphicResourceBase();

			virtual void Attach(IRender& device);
			virtual void Detach(IRender& device);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void OnResourceNotification(void* object, IRender::NOTIFY_TYPE notifyType);

		protected:
			IRender::Notifier* statusNotifier;
		};
	}
}


#endif // __GRAPHICRESOURCEBASE_H__