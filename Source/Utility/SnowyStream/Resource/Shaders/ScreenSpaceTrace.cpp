#include "../../../../Core/Template/TShaderGenerator.h"
#include "ScreenSpaceTrace.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::ShaderMacro;

ScreenSpaceTrace::ScreenSpaceTrace() {
	shaderTextBody = UnifyShaderCode(
		float v = 0;
	);
}

TObject<IReflect>& ScreenSpaceTrace::operator () (IReflect& reflect) {
	ReflectClass(ScreenSpaceTrace)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
	}

	return *this;
}
