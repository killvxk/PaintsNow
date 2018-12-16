// TranformComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-4
//

#ifndef __TRANSFORMCOMPONENTMODULE_H__
#define __TRANSFORMCOMPOENNTMODULE_H__

#include "TransformComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class TransformComponent;
		class TransformComponentModule : public ModuleImpl<TransformComponent> {
		public:
			TransformComponentModule(Engine& engine);

			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreate(IScript::Request& request);
			void RequestSetRotation(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent, ZFloat3& rotation);
			void RequestGetRotation(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent);
			void RequestSetScale(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent, ZFloat3& scale);
			void RequestGetScale(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent);
			void RequestSetTranslation(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent, ZFloat3& translation);
			void RequestGetTranslation(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent);
			void RequestLookAt(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent, ZFloat3& view, ZFloat3& direction, ZFloat3& up);
			void RequestGetQuickTranslation(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent);
		};
	}
}


#endif // __TRANSFORMCOMPONENTMODULE_H__