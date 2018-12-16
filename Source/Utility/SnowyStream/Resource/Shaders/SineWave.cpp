#include "../../../../Core/Template/TShaderGenerator.h"
#include "SineWave.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

SineWave::SineWave() {
	shaderTextBody = UnifyShaderCode(
		float v = 0;
	);
}

TObject<IReflect>& SineWave::operator () (IReflect& reflect) {
	ReflectClass(SineWave)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
	}

	return *this;
}
