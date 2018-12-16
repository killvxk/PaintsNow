// RenderableComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __RENDERABLECOMPONENT_H__
#define __RENDERABLECOMPONENT_H__

#include "../../Entity.h"
#include "../../Component.h"
#include "../../../../Core/Common/ZBatchRanger.h"

namespace PaintsNow {
	namespace NsMythForest {
		class RenderableComponent;
		class BatchComponent;
		class RenderableComponent : public Component {
		public:
			RenderableComponent();
			enum {
				// Render Queue Options
				RENDERABLE_COMPONENT_TRANSPARENT = COMPONENT_CUSTOM_BEGIN,
				RENDERABLE_COMPONENT_CLIPPING = COMPONENT_CUSTOM_BEGIN << 1,
				RENDERABLE_COMPONENT_DOUBLE_SIDE = COMPONENT_CUSTOM_BEGIN << 2,
				RENDERABLE_COMPONENT_DISABLE_DEPTH_TEST = COMPONENT_CUSTOM_BEGIN << 3,
				RENDERABLE_COMPONENT_DISABLE_DEPTH_WRITE = COMPONENT_CUSTOM_BEGIN << 4,
				RENDERABLE_COMPONENT_ANIMATION = COMPONENT_CUSTOM_BEGIN << 5,
				RENDERABLE_COMPONENT_CAST_SHADOW = COMPONENT_CUSTOM_BEGIN << 6,
				RENDERABLE_COMPONENT_RECEIVE_SHADOW = COMPONENT_CUSTOM_BEGIN << 7,
				RENDERABLE_COMPONENT_CLAMP_TO_VIEW = COMPONENT_CUSTOM_BEGIN << 8,
				RENDERABLE_COMPONENT_CUSTOM_BEGIN = COMPONENT_CUSTOM_BEGIN << 9
			};

			struct Instance {
				ZMatrixFloat4x4 worldTransform;
			};

			virtual bool UpdateRenderData() = 0;
			virtual void RenderInstance(IRender& render, IRender::Target* renderTarget, Instance& instance) = 0;
			virtual Tiny::FLAG GetEntityMask() const override final;
			virtual bool UpdateCommandCapture(ZBatchRanger& batchRanger, ZBatchRanger::CommandCapture& commandCapture);
		};
	}
}


#endif // __RENDERABLECOMPONENT_H__
