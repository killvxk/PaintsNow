// StandardPipeline.h
// Standard Physical Based Shader
// By PaintDream (paintdream@paintdream.com)
//

#ifndef __STANDARD_PIPELINE_H__
#define __STANDARD_PIPELINE_H__

#include "../../../../Core/Common/ZPipeline.h"
#include "../Shaders/StandardTransform.h"
#include "../Shaders/StandardParameter.h"
#include "../Shaders/DeferredCompact.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		// standard pbr deferred shading pipeline using ggx prdf
		class StandardPipeline : public ZPipeline {
		public:
			StandardPipeline();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			// Vertex shaders
			StandardTransform shaderTransform;
			// Fragment shaders
			StandardParameter shaderParameter;
			DeferredCompactEncode shaderCompactEncode;

		protected:
			bool enableSkinning;
			bool enableVertexColor;
			bool enableLightMap;
			bool enableShadowMap;
			bool enableTransparent;
			bool enableClipping;
			bool enableInstancing;
			bool enableBillboard;
		};
	}
}


#endif // __STANDARD_PIPELINE_H__