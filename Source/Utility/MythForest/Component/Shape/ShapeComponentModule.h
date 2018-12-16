// ShapeComponentModule.h
// By PaintDream (paintdream@paintdream.com)
// 2015-5-5
//

#ifndef __SHAPECOMPONENTMODULE_H__
#define __SHAPECOMPONENTMODULE_H__

#include "ShapeComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class Entity;
		class ShapeComponent;
		class ShapeComponentModule : public ModuleImpl<ShapeComponent> {
		public:
			ShapeComponentModule(Engine& engine);
			virtual ~ShapeComponentModule();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreate(IScript::Request& request);
			void RequestRebuild(IScript::Request& request, IScript::Delegate<ShapeComponent> shapeComponent, ZFloat4& color);
		};
	}
}


#endif // __SHAPECOMPONENTMODULE_H__