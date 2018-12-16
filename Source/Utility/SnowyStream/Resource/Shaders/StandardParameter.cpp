#include "../../../../Core/Template/TShaderGenerator.h"
#include "StandardParameter.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::ShaderMacro;

StandardParameter::StandardParameter() {
	shaderTextBody = UnifyShaderCode(
		vec4 albedo = baseColor.xyzw;
		if (enableAlbedoMap) {
			albedo = albedo.xyzw * texture(albedoMap, texCoord.xy);
		}

		alpha = albedo.w;
		vec3 normal = worldNormal.xyz;
		if (enableNormalMap) {
			vec4 bump = texture(normalMap, texCoord.xy);
			shininess = bump.z;

			bump.xyz = normalize(vec3(bump.x, bump.y, 1) * 2.0 - 1.0);
			normal = worldTangent.xyz * bump.x + worldBinormal.xyz * bump.y + worldNormal.xyz * bump.z;
		} else {
			shininess = 0.0;
		}

		if (enableParamMap) {
			vec4 param = texture(paramMap, texCoord.xy);
			metallic = param.x;
			roughness = param.y;
			occlusion = param.z;
		} else {
			metallic = 0.0;
			roughness = 1.0;
			occlusion = 1.0;
		}

		albedoColor = albedo.xyz;
	);
}

TObject<IReflect>& StandardParameter::operator () (IReflect& reflect) {
	ReflectClass(StandardParameter)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
		// Inputs:
		ReflectProperty(texCoord);
		ReflectProperty(worldNormal);
		ReflectProperty(worldBinormal);
		ReflectProperty(worldTangent);
		ReflectProperty(albedoMap);
		ReflectProperty(normalMap);
		ReflectProperty(paramMap);
		ReflectProperty(baseColor);

		// Outputs:
		ReflectProperty(alpha);
		ReflectProperty(metallic);
		ReflectProperty(roughness);
		ReflectProperty(occlusion);
		ReflectProperty(shininess);
		ReflectProperty(albedoColor);
	}

	return *this;
}
