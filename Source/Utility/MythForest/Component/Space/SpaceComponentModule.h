// SpaceComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-10
//

#ifndef __SPACECOMPONENTMODULE_H__
#define __SPACECOMPONENTMODULE_H__

#include "SpaceComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class Entity;
		class SpaceComponent;
		class SpaceComponentModule : public ModuleImpl<SpaceComponent> {
		public:
			SpaceComponentModule(Engine& engine);
			void RequestCreate(IScript::Request& request);
			void RequestInsertEntity(IScript::Request& request, IScript::Delegate<SpaceComponent> spaceComponent, IScript::Delegate<Entity> entity);
			void RequestRemoveEntity(IScript::Request& request, IScript::Delegate<SpaceComponent> spaceComponent, IScript::Delegate<Entity> entity);
			void RequestQueryEntities(IScript::Request& request, IScript::Delegate<SpaceComponent> spaceComponent, ZFloat3Pair& box, IScript::Request::Ref callback, uint32_t groupCount);

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual void ScriptUninitialize(IScript::Request& request);
		};
	}
}

#endif // __SPACECOMPONENTMODULE_H__