#include "../../../../Core/Template/TShaderGenerator.h"
#include "StandardTransform.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::ShaderMacro;

StandardTransform::StandardTransform() {
	shaderTextBody = UnifyShaderCode(
		vec4 position = vec4(0, 0, 0, 1);
		position.xyz = vertexPosition.xyz;
		worldPosition = mult_vec(modelViewMatrix, position.xyzw);
		rasterPosition = mult_vec(projectionMatrix, worldPosition.xyzw);

		vec3 normal = vec3(0, 0, 1);
		if (enableVertexNormal) {
			normal = mult_vec(normalMatrix, vertexNormal.xyz);
		}

		worldNormal.xyz = normal.xyz;

		vec4 color = vec4(1, 1, 1, 1);
		if (enableVertexColor) {
			color = vertexColor;
		}

		baseColor = color;
	);
}

TObject<IReflect>& StandardTransform::operator () (IReflect& reflect) {
	ReflectClass(StandardTransform)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(modelViewMatrix);
		ReflectProperty(projectionMatrix);
		ReflectProperty(normalMatrix);
		ReflectProperty(rasterPosition);
		ReflectProperty(vertexPosition);
		ReflectProperty(vertexNormal);
		ReflectProperty(vertexColor);
		ReflectProperty(worldPosition);
		ReflectProperty(worldNormal);
		ReflectProperty(baseColor);

		ReflectProperty(enableVertexNormal);
		ReflectProperty(enableVertexColor);
	}

	return *this;
}