// RenderableComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __RENDERABLECOMPONENTMODULE_H__
#define __RENDERABLECOMPONENTMODULE_H__

#include "RenderableComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class RenderableComponent;
		class RenderableComponentModule : public ModuleImpl<RenderableComponent> {
		public:
			RenderableComponentModule(Engine& engine);
		};
	}
}


#endif // __RENDERABLECOMPONENTMODULE_H__
