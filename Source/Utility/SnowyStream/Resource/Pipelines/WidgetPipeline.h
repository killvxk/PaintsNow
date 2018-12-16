// WidgetPipeline.h
// Widget Physical Based Shader
// By PaintDream (paintdream@paintdream.com)
//

#ifndef __WIDGET_PIPELINE_H__
#define __WIDGET_PIPELINE_H__

#include "../../../../Core/Common/ZPipeline.h"
#include "../Shaders/WidgetTransform.h"
#include "../Shaders/WidgetShading.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		// standard pbr deferred shading pipeline using ggx prdf
		class WidgetPipeline : public ZPipeline {
		public:
			WidgetPipeline();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		public:
			// Vertex shaders
			WidgetTransform widgetTransform;

			// Fragment shaders
			WidgetShading widgetShading;
		};
	}
}


#endif // __WIDGET_PIPELINE_H__