// DeferredCompact.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __DEFERREDCOMPACT_H__
#define __DEFERREDCOMPACT_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class DeferredCompactEncode : public IShader {
		public:
			DeferredCompactEncode();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			// inputs
			TShaderVariable<ZFloat3, IShaderVariableBase::IMPORT> worldNormal;
			TShaderVariable<ZFloat3, IShaderVariableBase::IMPORT> albedoColor;
			TShaderVariable<float, IShaderVariableBase::IMPORT> metallic;
			TShaderVariable<float, IShaderVariableBase::IMPORT> roughness;
			TShaderVariable<float, IShaderVariableBase::IMPORT> emission;
			TShaderVariable<float, IShaderVariableBase::IMPORT> occlusion;
			TShaderVariable<float, IShaderVariableBase::IMPORT> alpha;
			TShaderVariable<float, IShaderVariableBase::IMPORT> depth;

			// outputs
			TShaderVariable<ZFloat4, IShaderVariableBase::OUTPUT_TARGET> BaseColor;
			TShaderVariable<ZFloat4, IShaderVariableBase::OUTPUT_TARGET> NormalDepth;
			TShaderVariable<ZFloat4, IShaderVariableBase::OUTPUT_TARGET> Material;
		};

		class DeferredCompactDecode : public IShader {
		public:
			DeferredCompactDecode();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			// input
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_VARYING> rasterCoord;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE> BaseColor;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE> NormalDepth;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE> Material;
			TShaderVariable<ZMatrixFloat4x4, IShaderVariableBase::INPUT_UNIFORM> inverseModelViewProjectionMatrix;

			// outputs
			TShaderVariable<ZFloat3, IShaderVariableBase::LOCAL> worldNormal;
			TShaderVariable<ZFloat3, IShaderVariableBase::LOCAL> worldPosition;
			TShaderVariable<ZFloat3, IShaderVariableBase::LOCAL> albedoColor;
			TShaderVariable<float, IShaderVariableBase::LOCAL> metallic;
			TShaderVariable<float, IShaderVariableBase::LOCAL> roughness;
			TShaderVariable<float, IShaderVariableBase::LOCAL> emission;
			TShaderVariable<float, IShaderVariableBase::LOCAL> occlusion;
			TShaderVariable<float, IShaderVariableBase::LOCAL> alpha;
			TShaderVariable<float, IShaderVariableBase::LOCAL> depth;
		};
	}
}


#endif // __DEFERREDCOMPACT_H__
