// GeometryBufferRenderStage.h
// PaintDream (paintdream@paintdream.com)
// 2018-9-11
//

#ifndef __GEOMETRYBUFFERRENDERSTAGE_H__
#define __GEOMETRYBUFFERRENDERSTAGE_H__

#include "../RenderStage.h"
#include "../RenderPort/RenderPortCommandQueue.h"
#include "../RenderPort/RenderPortRenderTarget.h"

namespace PaintsNow {
	namespace NsMythForest {
		class GeometryBufferRenderStage : public RenderStage {
		public:
			GeometryBufferRenderStage();

			virtual TObject<IReflect>& operator () (IReflect& reflect) override;

			RenderPortCommandQueue Primitives; // input primitives

			RenderPortRenderTarget BaseColor;
			RenderPortRenderTarget NormalDepth;
			RenderPortRenderTarget Material;
		};
	}
}

#endif // __GEOMETRYBUFFERRENDERSTAGE_H__