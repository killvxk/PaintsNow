#include "../../../../Core/Template/TShaderGenerator.h"
#include "StandardSkinning.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::ShaderMacro;

StandardSkinning::StandardSkinning() {
	shaderTextBody = UnifyShaderCode(
		float v = 0;
	);
}

TObject<IReflect>& StandardSkinning::operator () (IReflect& reflect) {
	ReflectClass(StandardSkinning)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
	}

	return *this;
}
