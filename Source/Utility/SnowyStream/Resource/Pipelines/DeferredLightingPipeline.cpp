#include "DeferredLightingPipeline.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

DeferredLightingPipeline::DeferredLightingPipeline() {}

TObject<IReflect>& DeferredLightingPipeline::operator () (IReflect& reflect) {
	ReflectClass(DeferredLightingPipeline)[Interface(ZPipeline)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(shaderTransform)[ShaderType = IRender::VERTEX_SHADER];
		ReflectProperty(shaderCompactDecode)[ShaderType = IRender::FRAGMENT_SHADER];
		ReflectProperty(shaderLighting)[ShaderType = IRender::FRAGMENT_SHADER];
	}

	return *this;
}