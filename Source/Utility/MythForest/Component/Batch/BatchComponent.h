// BatchComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __BATCHCOMPONENT_H__
#define __BATCHCOMPONENT_H__

#include "../../Entity.h"
#include "../Renderable/RenderableComponent.h"
#include "../../../../Core/Common/ZBatchRanger.h"
#include "../../../SnowyStream/Resource/ShaderResource.h"

namespace PaintsNow {
	namespace NsMythForest {
		class BatchComponent : public RenderableComponent {
		public:
			BatchComponent(TShared<NsSnowyStream::ShaderResource> shader);
			virtual ~BatchComponent();
			virtual bool UpdateRenderData() override final;
			virtual void RenderInstance(IRender& render, IRender::Target* renderTarget, Instance& instance) override final;

			TShared<NsSnowyStream::ShaderResource> GetBaseShaderResource() const;
			void BeginCapture(IRender& render);
			void DoCapture(IRender& render, TShared<RenderableComponent> renderableComponent);
			void FlushCapture(IRender& render);
			void EndCapture(IRender& render);
			void Cleanup(IRender& render);

		protected:
			std::vector<ZBatchRanger::CommandCapture> commandCaptures;
			ZBatchRanger::CommandQueue commandQueue;
			TShared<NsSnowyStream::ShaderResource> baseShaderResource;
			TShared<NsSnowyStream::ShaderResource> batchedShaderResource;
			ZBatchRanger batchRanger;
		};
	}
}


#endif // __BATCHCOMPONENT_H__
