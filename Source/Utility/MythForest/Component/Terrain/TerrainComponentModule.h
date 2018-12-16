// TerrainComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __TERRAINCOMPONENTMODULE_H__
#define __TERRAINCOMPONENTMODULE_H__

#include "TerrainComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class TerrainComponent;
		class TerrainComponentModule : public ModuleImpl<TerrainComponent> {
		public:
			TerrainComponentModule(Engine& engine);

			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreate(IScript::Request& request, IScript::Delegate<NsSnowyStream::TerrainResource> terrainResource);
			void RequestRebuild(IScript::Request& request, IScript::Delegate<TerrainComponent> terrainComponent);
		};
	}
}


#endif // __TERRAINCOMPONENTMODULE_H__
