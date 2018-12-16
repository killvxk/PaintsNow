// ModelComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __MODELCOMPONENTMODULE_H__
#define __MODELCOMPONENTMODULE_H__

#include "ModelComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class ModelComponent;
		class ModelComponentModule : public ModuleImpl<ModelComponent> {
		public:
			ModelComponentModule(Engine& engine);

			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreate(IScript::Request& request, IScript::Delegate<NsSnowyStream::ModelResource> modelResource);
			void RequestRebuild(IScript::Request& request, IScript::Delegate<ModelComponent> modelComponent);
		};
	}
}


#endif // __MODELCOMPONENTMODULE_H__
