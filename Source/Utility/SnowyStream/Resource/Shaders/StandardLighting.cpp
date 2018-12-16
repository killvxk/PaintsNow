#include "../../../../Core/Template/TShaderGenerator.h"
#include "StandardLighting.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::ShaderMacro;

StandardLighting::StandardLighting() {
	std::vector<Light> lights(lightCount); // Make compiler happy
	shaderTextBody = UnifyShaderCode(
		const float PI = 3.1415926;
		float GAMMA = 2.2;
		const vec3 minSpecularColor = vec3(0.04, 0.04, 0.04);
		vec3 diffuseColor = vec3(0, 0, 0);
		vec3 specularColor = vec3(0, 0, 0);
		vec3 linearColor = vec3(0, 0, 0);
		float ao = occlusion;
		linearColor.xyz = pow(albedoColor.xyz, GAMMA);
		diffuseColor = (linearColor.xyz - linearColor.xyz * (float)metallic) / PI;
		specularColor = lerp(minSpecularColor.xyz, linearColor.xyz, (float)metallic);
		vec3 V = normalize(viewPosition.xyz - worldPosition.xyz);
		vec3 N = worldNormal.xyz;
		float NoV = dot(V.xyz, N.xyz);

		float em = (float)emission;
		MainColor.w = 1;
		MainColor.xyz = albedoColor.xyz * em + ambientExposure.xyz * ao;

		for (int i = 0; i < lightCount; i++) {
			vec4 lightPos = lights[i].position;
			vec3 lightPart = lightPos.xyz;
			vec3 path = lightPart - worldPosition.xyz * lightPos.w;
			float intensity = saturate(1.0 - (float)dot(path.xyz, path.xyz) * lights[i].colorFade.w);
			vec3 lightColor = lights[i].colorFade.xyz;
			lightColor = lightColor * (float)intensity;
			vec3 L = normalize(path);
			vec3 H = normalize(L + V);

			float NoH = saturate(dot(N.xyz, H.xyz));
			float NoL = saturate(dot(N.xyz, L.xyz));
			float VoH = saturate(dot(V.xyz, H.xyz));

			float microShadow = saturate(abs(NoL) + ao * ao * 2 - 1);
			float D = 0.0;
			{
				float p = roughness * roughness;
				p = p * p;
				float q = (NoH * p - NoH) * NoH + 1.0;
				D = p / (q * q + 0.0001);
			}

			float G = 0.0;
			{
				G = 0.25 / max(NoL, NoV);
			}

			vec3 F = vec3(0.0, 0.0, 0.0);
			{
				const vec3 ones = vec3(1.0, 1.0, 1.0);
				F = specularColor + (ones * saturate(50.0 * specularColor.y) - specularColor) * exp2(VoH *(-5.55473 * VoH - 6.98316));
			}

			MainColor.xyz = (diffuseColor + F.xyz * (D * G)) * lightColor * NoL * microShadow + MainColor.xyz;
		}

		MainColor.xyz = pow(MainColor.xyz * ambientExposure.w, 1 / GAMMA);
	);
}

TObject<IReflect>& StandardLighting::operator () (IReflect& reflect) {
	ReflectClass(StandardLighting)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(worldPosition);
		ReflectProperty(worldNormal);
		ReflectProperty(viewPosition);
		ReflectProperty(albedoColor);
		ReflectProperty(metallic);
		ReflectProperty(roughness);
		ReflectProperty(occlusion);
		ReflectProperty(emission);
		ReflectProperty(ambientExposure);
		ReflectProperty(lights);
		ReflectProperty(lightCount);
		ReflectProperty(MainColor);
	}

	return *this;
}
