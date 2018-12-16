// MultiHashTrace.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __MULTIHASHTRACE_H__
#define __MULTIHASHTRACE_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class MultiHashTrace : public IShader {
		public:
			MultiHashTrace();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			// Control Variables
			TShaderVariable<ZFloat4> sigmaTransRate;
			TShaderVariable<ZFloat4> texCoord;

			// Input Parameters
			TShaderVariable<ZFloat3> worldPosition;
			TShaderVariable<ZFloat3> worldNormal;
			TShaderVariable<ZFloat3> viewPosition;
			TShaderVariable<ZFloat3> albedoColor;
			TShaderVariable<float> metallic;
			TShaderVariable<float> roughness;
			TShaderVariable<float> occlusion;
			TShaderVariable<float> emission;

			// State texture (RGBD)
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE> inputState;
			TShaderVariable<ZFloat4> outputState;

			// Transitions ...
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE> transState;
			TShaderVariable<ZMatrixFloat4x4> transMatrix;
			TShaderVariable<ZMatrixFloat4x4> transToWorldMatrix;
			TShaderVariable<ZMatrixFloat4x4> inputToWorldMatrix;
		};
	}
}


#endif // __MULTIHASHTRACE_H__
