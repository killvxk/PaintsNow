#include "../../../../Core/Template/TShaderGenerator.h"
#include "ShadowMap.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

ShadowMap::ShadowMap() {
	shaderTextBody = UnifyShaderCode(
		float v = 0;
	);
}

TObject<IReflect>& ShadowMap::operator () (IReflect& reflect) {
	ReflectClass(ShadowMap)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
	}

	return *this;
}
