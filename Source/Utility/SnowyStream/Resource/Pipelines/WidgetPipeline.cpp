#include "WidgetPipeline.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

WidgetPipeline::WidgetPipeline() {
}

TObject<IReflect>& WidgetPipeline::operator () (IReflect& reflect) {
	ReflectClass(WidgetPipeline)[Interface(ZPipeline)];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(widgetTransform)[ShaderType = IRender::VERTEX_SHADER];
		ReflectProperty(widgetShading)[ShaderType = IRender::FRAGMENT_SHADER];
	}

	return *this;
}