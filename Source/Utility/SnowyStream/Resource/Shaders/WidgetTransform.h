// WidgetTransform.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __WIDGETTRANSFORM_H__
#define __WIDGETTRANSFORM_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class WidgetTransform : public IShader {
		public:
			WidgetTransform();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		public:
			// Input attribs
			TShaderVariable<ZFloat3, IShaderVariableBase::INPUT_ATTRIB_BUFFER> unitPositionTexCoord;

			// Input uniforms
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM> inputPositionRect;

			// Output vars
			TShaderVariable<ZFloat4, IShaderVariableBase::OUTPUT_POSITION> position;
			TShaderVariable<ZFloat2, IShaderVariableBase::OUTPUT_VARYING> texCoord;
		};
	}
}


#endif // __WIDGETTRANSFORM_H__