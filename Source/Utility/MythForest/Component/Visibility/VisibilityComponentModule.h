// VisibilityComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __VISIBILITYCOMPONENTMODULE_H__
#define __VISIBILITYCOMPONENTMODULE_H__

#include "VisibilityComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class VisibilityComponent;
		class VisibilityComponentModule : public ModuleImpl<VisibilityComponent> {
		public:
			VisibilityComponentModule(Engine& engine);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreate(IScript::Request& request);
			void RequestRebuild(IScript::Request& request, IScript::Delegate<VisibilityComponent> visibilityComponent);
		};
	}
}


#endif // __VISIBILITYCOMPONENTMODULE_H__
