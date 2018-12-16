// StandardTransform.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __STANDARDTRANSFORM_H__
#define __STANDARDTRANSFORM_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class StandardTransform : public IShader {
		public:
			StandardTransform();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			TShaderVariable<ZMatrixFloat4x4, IShaderVariableBase::INPUT_UNIFORM> modelViewMatrix;
			TShaderVariable<ZMatrixFloat4x4, IShaderVariableBase::INPUT_UNIFORM> projectionMatrix;
			TShaderVariable<ZMatrixFloat3x3, IShaderVariableBase::INPUT_UNIFORM> normalMatrix;

			TShaderVariable<ZFloat4, IShaderVariableBase::OUTPUT_POSITION> rasterPosition;
			TShaderVariable<ZFloat3, IShaderVariableBase::INPUT_VARYING> vertexPosition;
			TShaderVariable<ZFloat3, IShaderVariableBase::INPUT_VARYING> vertexNormal;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_VARYING> vertexColor;

			TShaderVariable<ZFloat4, IShaderVariableBase::LOCAL> worldPosition;
			TShaderVariable<ZFloat4, IShaderVariableBase::LOCAL> worldNormal;
			TShaderVariable<ZFloat4, IShaderVariableBase::LOCAL> baseColor;

			bool enableVertexNormal;
			bool enableVertexColor;
		};
	}
}


#endif // __STANDARDTRANSFORM_H__