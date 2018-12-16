// LightComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __LIGHTCOMPONENTMODULE_H__
#define __LIGHTCOMPONENTMODULE_H__

#include "LightComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class LightComponent;
		class LightComponentModule : public ModuleImpl<LightComponent> {
		public:
			LightComponentModule(Engine& engine);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreate(IScript::Request& request);
			void RequestSetLightColor(IScript::Request& request, IScript::Delegate<LightComponent> lightComponent, ZFloat4& color);
		};
	}
}


#endif // __LIGHTCOMPONENTMODULE_H__
