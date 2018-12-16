#include "../../../../Core/Template/TShaderGenerator.h"
#include "DeferredCompact.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::ShaderMacro;

DeferredCompactEncode::DeferredCompactEncode() {
	shaderTextBody = UnifyShaderCode(
		vec4 normalDepth = vec4(0, 0, 0, depth);
		normalDepth.xyz = worldNormal.xyz;
		NormalDepth = _EncodeNormalDepth(normalDepth.xyzw);
		BaseColor.xyz = albedoColor;
		BaseColor.w = alpha;
		Material = vec4(metallic, roughness, emission, occlusion);
	);
}

TObject<IReflect>& DeferredCompactEncode::operator () (IReflect& reflect) {
	ReflectClass(DeferredCompactEncode)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
		// inputs
		ReflectProperty(worldNormal);
		ReflectProperty(albedoColor);
		ReflectProperty(metallic);
		ReflectProperty(roughness);
		ReflectProperty(emission);
		ReflectProperty(occlusion);
		ReflectProperty(alpha);
		ReflectProperty(depth);

		ReflectProperty(NormalDepth);
		ReflectProperty(BaseColor);
		ReflectProperty(Material);
	}

	return *this;
}

DeferredCompactDecode::DeferredCompactDecode() {
	shaderTextBody = UnifyShaderCode(
		vec4 normalDepth = texture(NormalDepth, rasterCoord.xy);
		normalDepth = _DecodeNormalDepth(normalDepth.xyzw);
		vec4 albedoAlpha = texture(BaseColor, rasterCoord.xy);
		vec4 material = texture(Material, rasterCoord.xy);

		const vec3 offset = vec3(1.0, 1.0, 1.0);
		const float scale = 2.0;
		alpha = albedoAlpha.w;
		vec4 position = vec4(rasterCoord.x, rasterCoord.y, depth, 1);
		position.xyz = rasterCoord.xyz * scale - offset.xyz;
		position.xyzw = mult_vec(inverseModelViewProjectionMatrix, position.xyzw);
		worldPosition = position.xyz / position.w;

		worldNormal = normalDepth.xyz;
		albedoColor = albedoAlpha.xyz;
		depth = normalDepth.w;

		metallic = material.x;
		roughness = material.y;
		emission = material.z;
		occlusion = material.w;
	);
}

TObject<IReflect>& DeferredCompactDecode::operator () (IReflect& reflect) {
	ReflectClass(DeferredCompactDecode)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(rasterCoord);
		ReflectProperty(NormalDepth);
		ReflectProperty(BaseColor);
		ReflectProperty(Material);
		ReflectProperty(inverseModelViewProjectionMatrix);
		
		ReflectProperty(worldNormal);
		ReflectProperty(worldPosition);
		ReflectProperty(albedoColor);
		ReflectProperty(metallic);
		ReflectProperty(roughness);
		ReflectProperty(emission);
		ReflectProperty(occlusion);
		ReflectProperty(alpha);
		ReflectProperty(depth);
	}

	return *this;
}