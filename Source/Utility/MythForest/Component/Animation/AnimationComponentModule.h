// AnimationComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __ANIMATIONCOMPONENTMODULE_H__
#define __ANIMATIONCOMPONENTMODULE_H__

#include "AnimationComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class AnimationComponentModule : public ModuleImpl<AnimationComponent> {
		public:
			AnimationComponentModule(Engine& engine);

			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreate(IScript::Request& request);
			void RequestRebuild(IScript::Request& request, IScript::Delegate<AnimationComponent> animationComponent);
		};
	}
}


#endif // __ANIMATIONCOMPONENTMODULE_H__
