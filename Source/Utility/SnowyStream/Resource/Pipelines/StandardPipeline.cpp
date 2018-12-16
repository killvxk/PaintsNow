#include "StandardPipeline.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

StandardPipeline::StandardPipeline() {}

TObject<IReflect>& StandardPipeline::operator () (IReflect& reflect) {
	ReflectClass(StandardPipeline)[Interface(ZPipeline)];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(shaderTransform)[ShaderType = IRender::VERTEX_SHADER];
		ReflectProperty(shaderParameter)[ShaderType = IRender::FRAGMENT_SHADER];
		ReflectProperty(shaderCompactEncode)[ShaderType = IRender::FRAGMENT_SHADER];

		ReflectProperty(enableSkinning);
		ReflectProperty(enableVertexColor);
		ReflectProperty(enableLightMap);
		ReflectProperty(enableShadowMap);
		ReflectProperty(enableTransparent);
		ReflectProperty(enableClipping);
		ReflectProperty(enableInstancing);
		ReflectProperty(enableBillboard);
	}

	return *this;
}