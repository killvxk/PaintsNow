// SurfaceComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __SURFACECOMPONENTMODULE_H__
#define __SURFACECOMPONENTMODULE_H__

#include "SurfaceComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class SurfaceComponent;
		class SurfaceComponentModule : public ModuleImpl<SurfaceComponent> {
		public:
			SurfaceComponentModule(Engine& engine);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreate(IScript::Request& request);
			void RequestRebuild(IScript::Request& request, IScript::Delegate<SurfaceComponent> surfaceComponent);
		};
	}
}


#endif // __SURFACECOMPONENTMODULE_H__
