// DeferredLightingRenderStage.h
// PaintDream (paintdream@paintdream.com)
// 2018-9-11
//

#ifndef __DEFERREDLIGHTINGRENDERSTAGE_H__
#define __DEFERREDLIGHTINGRENDERSTAGE_H__

#include "../RenderStage.h"
#include "../RenderPort/RenderPortRenderTarget.h"
#include "../RenderPort/RenderPortLightSource.h"

namespace PaintsNow {
	namespace NsMythForest {
		class DeferredLightingRenderStage : public RenderStage {
		public:
			DeferredLightingRenderStage();
			virtual TObject<IReflect>& operator () (IReflect& reflect) override;

			RenderPortLightSource LightSource;

			RenderPortRenderTarget BaseColor;
			RenderPortRenderTarget NormalDepth;
			RenderPortRenderTarget Material;
		};
	}
}

#endif // __DEFERREDLIGHTINGRENDERSTAGE_H__