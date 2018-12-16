// SurfaceComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __SURFACECOMPONENT_H__
#define __SURFACECOMPONENT_H__

#include "../../Entity.h"
#include "../Renderable/RenderableComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class SurfaceComponent : public RenderableComponent {
		public:
			SurfaceComponent();
			virtual bool UpdateRenderData() override;
			virtual void RenderInstance(IRender& render, IRender::Target* renderTarget, Instance& instance) override;
		};
	}
}


#endif // __SURFACECOMPONENT_H__
