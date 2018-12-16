#include "../../../../Core/Template/TShaderGenerator.h"
#include "WidgetTransform.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::ShaderMacro;

WidgetTransform::WidgetTransform() {
	shaderTextBody = UnifyShaderCode(
		position = vec4(0, 0, unitPositionTexCoord.z, 1);
		position.xy = lerp(inputPositionRect.xy, inputPositionRect.zw, unitPositionTexCoord.xy);
		texCoord.xy = unitPositionTexCoord.xy;
	);
}

TObject<IReflect>& WidgetTransform::operator () (IReflect& reflect) {
	ReflectClass(WidgetTransform)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(unitPositionTexCoord)[Runtime];
		ReflectProperty(inputPositionRect)[Runtime];
		ReflectProperty(position)[Runtime];
		ReflectProperty(texCoord)[Runtime];
	}

	return *this;
}