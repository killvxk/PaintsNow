// RenderPortCommandQueue.h
// PaintDream (paintdream@paintdream.com)
// 2018-10-15
//

#ifndef __RENDERPORTCOMMANDQUEUE_H__
#define __RENDERPORTCOMMANDQUEUE_H__

#include "../RenderPort.h"
#include "../../Renderable/RenderableComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class BatchComponent;
		class RenderPortCommandQueue : public RenderPort {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect) override;

			struct RenderableElement : public RenderableComponent::Instance {
				TShared<RenderableComponent> renderableComponent;
				TShared<BatchComponent> batchComponent;
			};

			IRender::Target* BeginTransaction();
			void EndTransaction();

			TShared<NsSnowyStream::RenderTargetResource> renderTargetResource;
		};
	}
}

#endif // __RENDERPORTCOMMANDQUEUE_H__