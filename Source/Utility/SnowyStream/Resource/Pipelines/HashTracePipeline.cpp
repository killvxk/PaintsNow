#include "HashTracePipeline.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

HashTracePipeline::HashTracePipeline() {}

TObject<IReflect>& HashTracePipeline::operator () (IReflect& reflect) {
	ReflectClass(HashTracePipeline)[Interface(ZPipeline)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(shaderTransform)[ShaderType = IRender::VERTEX_SHADER];
		ReflectProperty(shaderCompactDecode)[ShaderType = IRender::FRAGMENT_SHADER];
		ReflectProperty(shaderLighting)[ShaderType = IRender::FRAGMENT_SHADER];
	}

	return *this;
}