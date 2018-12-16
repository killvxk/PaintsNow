// WidgetShading.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __WIDGETSHADING_H__
#define __WIDGETSHADING_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class WidgetShading : public IShader {
		public:
			WidgetShading();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		public:
			// varyings
			TShaderVariable<ZFloat2, IShaderVariableBase::INPUT_VARYING> texCoord;

			// inputs
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM_TEXTURE> mainTexture;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM> inputTexCoordRect;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM> inputTexCoordMark;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM> inputTexCoordScale;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM> tintColor;

			// targets
			TShaderVariable<ZFloat4, IShaderVariableBase::OUTPUT_TARGET> target;
		};
	}
}


#endif // __WIDGETSHADING_H__