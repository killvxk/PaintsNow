#include "../../../../Core/Template/TShaderGenerator.h"
#include "WidgetShading.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::ShaderMacro;

WidgetShading::WidgetShading() {
	shaderTextBody = UnifyShaderCode(
		vec4 zeros = vec4(0.0, 0.0, 0.0, 0.0);
		vec4 offset = max(texCoord.xyxy - inputTexCoordMark.xyzw, zeros.xyzw);
		vec2 minCoord = min(texCoord.xy, inputTexCoordMark.xy);
		vec2 newCoord = minCoord * inputTexCoordScale.xy;
		newCoord.xy += inputTexCoordScale.zw * (offset.xy - offset.zw);
		newCoord.xy += inputTexCoordScale.xy * offset.zw;
		vec2 coord = lerp(inputTexCoordRect.xy, inputTexCoordRect.zw, newCoord.xy);
		target = texture(mainTexture, coord.xy) * tintColor.xyzw;
	);
}

TObject<IReflect>& WidgetShading::operator () (IReflect& reflect) {
	ReflectClass(WidgetShading)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(texCoord)[Runtime];
		ReflectProperty(mainTexture)[Runtime];
		ReflectProperty(inputTexCoordRect)[Runtime];
		ReflectProperty(inputTexCoordScale)[Runtime];
		ReflectProperty(inputTexCoordMark)[Runtime];
		ReflectProperty(tintColor)[Runtime];
		ReflectProperty(target)[Runtime];
	}

	return *this;
}