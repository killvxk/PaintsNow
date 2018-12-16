// StandardLighting.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __STANDARDLIGHTING_H__
#define __STANDARDLIGHTING_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class StandardLighting : public IShader {
		public:
			StandardLighting();
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			struct Light : public IReflectObjectComplex {
				Light() {
				}

				virtual TObject<IReflect>& operator () (IReflect& reflect) {
					ReflectClass(Light)[Constructable];
					if (reflect.IsReflectProperty()) {
						ReflectProperty(position);
						ReflectProperty(colorFade);
					}

					return *this;
				}

				TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM> position;
				TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM> colorFade;
			};

		protected:
			TShaderVariable<ZFloat3, IShaderVariableBase::IMPORT> worldPosition;
			TShaderVariable<ZFloat3, IShaderVariableBase::IMPORT> worldNormal;
			TShaderVariable<ZFloat3, IShaderVariableBase::IMPORT> viewPosition;
			TShaderVariable<ZFloat3, IShaderVariableBase::IMPORT> albedoColor;
			TShaderVariable<float, IShaderVariableBase::IMPORT> metallic;
			TShaderVariable<float, IShaderVariableBase::IMPORT> roughness;
			TShaderVariable<float, IShaderVariableBase::IMPORT> occlusion;
			TShaderVariable<float, IShaderVariableBase::IMPORT> emission;
			TShaderVariable<ZFloat4, IShaderVariableBase::INPUT_UNIFORM> ambientExposure;
			TShaderVariable<IReflectObject*, IShaderVariableBase::INPUT_UNIFORM_BUFFER> lights;
			SWITCH_TYPE lightCount;

			TShaderVariable<ZFloat4, IShaderVariableBase::OUTPUT_TARGET> MainColor;
		};
	}
}


#endif // __STANDARDLIGHTING_H__
