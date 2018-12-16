#include "../../../../Core/Template/TShaderGenerator.h"
#include "MultiHashTrace.h"
#include "../../../../Core/Template/TShaderMacro.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::ShaderMacro;

MultiHashTrace::MultiHashTrace() {
	shaderTextBody = UnifyShaderCode(
		const float PI = 3.1415926;
		/* sample input */
		vec4 previous = texture(inputState, texCoord.xy);
		/* compute target coord */
		vec4 point = vec4(texCoord.x, texCoord.y, previous.w, 1);
		/* convert to trans space */
		point = mult_vec(transMatrix, point.xyzw);
		point.xy = point.xy / point.w;
		vec4 deltaLight = vec4(0, 0, 0, 0.0001);
		for (float i = -sigmaTransRate.x; i < sigmaTransRate.x; i += sigmaTransRate.z) {
			for (float j = -sigmaTransRate.y; j < sigmaTransRate.y; j += sigmaTransRate.z) {
				/* compute source coords */
				vec2 offset = vec2(i, j);
				vec4 coordNeighbor = vec4(texCoord.x, texCoord.y, 0, 1);
				coordNeighbor.xy += offset.xy;
				vec4 coordTrans = vec4(point.x, point.y, 0, 1);
				coordTrans.xy += offset.xy;

				/* sample light info */
				vec4 neighbor = texture(inputState, coordNeighbor.xy);
				vec4 trans = texture(transState, coordTrans.xy);
				coordNeighbor.z = neighbor.w;
				coordTrans.z = trans.w;

				/* compute guassian light transporting ...*/
				vec4 neighborPosition = mult_vec(inputToWorldMatrix, coordNeighbor.xyzw);
				vec4 transPosition = mult_vec(transToWorldMatrix, coordTrans.xyzw);

				deltaLight.xyz += neighbor.xyz;

				/* trans point part */
				float tnormDist = dot(worldPosition.xyz - transPosition.xyz / transPosition.w, worldNormal.xyz);
				float tweight = exp(-max(0, tnormDist) * sigmaTransRate.w) * step(0.001, tnormDist);
				deltaLight.xyz += trans.xyz * tweight;
				deltaLight.w += tweight;

				/* neibor point part */
				float nnormDist = dot(worldPosition.xyz - neighborPosition.xyz / neighborPosition.w, worldNormal.xyz);
				float nweight = exp(-max(0, nnormDist) * sigmaTransRate.w) * step(0.001, nnormDist);
				deltaLight.xyz += neighbor.xyz * nweight;
				deltaLight.w += nweight;
			}
		}

		/* reform light data */
		/* TODO: take material into consideration ... */
		outputState.xyz = max(previous.xyz, deltaLight.xyz / deltaLight.w / PI);
		outputState.w = previous.w;
	);
}

TObject<IReflect>& MultiHashTrace::operator () (IReflect& reflect) {
	ReflectClass(MultiHashTrace)[Interface(IShader)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(sigmaTransRate);
		ReflectProperty(worldPosition);
		ReflectProperty(worldNormal);
		ReflectProperty(viewPosition);
		ReflectProperty(albedoColor);
		ReflectProperty(metallic);
		ReflectProperty(roughness);
		ReflectProperty(occlusion);
		ReflectProperty(emission);
		ReflectProperty(inputState);
		ReflectProperty(outputState);
		ReflectProperty(transState);
		ReflectProperty(transMatrix);
		ReflectProperty(inputToWorldMatrix);
		ReflectProperty(transToWorldMatrix);
	}

	return *this;
}
