// StandardParameter.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __STANDARDPARAMETER_H__
#define __STANDARDPARAMETER_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class StandardParameter : public IShader {
		public:
			StandardParameter();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			// Inputs:
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_VARYING> texCoord;
			TShaderVariable<ZFloat3, IShaderVariableBase::INPUT_VARYING> worldNormal;
			TShaderVariable<ZFloat3, IShaderVariableBase::INPUT_VARYING> worldBinormal;
			TShaderVariable<ZFloat3, IShaderVariableBase::INPUT_VARYING> worldTangent;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE> albedoMap;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE> normalMap;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE> paramMap;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM> baseColor;

			// Outputs:
			TShaderVariable<float, IShaderVariableBase::LOCAL> alpha;
			TShaderVariable<float, IShaderVariableBase::LOCAL> metallic;
			TShaderVariable<float, IShaderVariableBase::LOCAL> roughness;
			TShaderVariable<float, IShaderVariableBase::LOCAL> occlusion;
			TShaderVariable<float, IShaderVariableBase::LOCAL> shininess;
			TShaderVariable<ZFloat3, IShaderVariableBase::LOCAL> albedoColor;
			/*
			TShaderVariable<float> NoL;
			TShaderVariable<float> NoH;
			TShaderVariable<float> NoV;
			TShaderVariable<ZFloat3> diffuseColor;
			TShaderVariable<ZFloat3> specularColor;*/

			// Switches:
			bool enableAlbedoMap;
			bool enableNormalMap;
			bool enableParamMap;
			bool enableComputedTangent;
		};
	}
}


#endif // __STANDARDPARAMETER_H__
