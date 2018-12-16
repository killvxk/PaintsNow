// HashTrace.h
// Standard Physical Based Shader
// By PaintDream (paintdream@paintdream.com)
//

#ifndef __HASHTRACE_PIPELINE_H__
#define __HASHTRACE_PIPELINE_H__

#include "../../../../Core/Common/ZPipeline.h"
#include "../Shaders/StandardTransform.h"
#include "../Shaders/DeferredCompact.h"
#include "../Shaders/StandardLighting.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		// standard pbr deferred shading pipeline using ggx prdf
		class HashTracePipeline : public ZPipeline {
		public:
			HashTracePipeline();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			// Vertex shaders
			StandardTransform shaderTransform;
			// Fragment shaders
			DeferredCompactDecode shaderCompactDecode;
			StandardLighting shaderLighting;
		};
	}
}


#endif // __HASHTRACE_PIPELINE_H__
