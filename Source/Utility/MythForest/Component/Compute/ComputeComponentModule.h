// ComputeComponentModule.h
// By PaintDream (paintdream@paintdream.com)
// 2015-5-5
//

#ifndef __COMPUTECOMPONENTMODULE_H__
#define __COMPUTECOMPONENTMODULE_H__

#include "ComputeComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class Entity;
		class ComputeComponent;
		class ComputeRoutine;
		class ComputeComponentModule : public ModuleImpl<ComputeComponent> {
		public:
			ComputeComponentModule(Engine& engine);
			virtual ~ComputeComponentModule();
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreate(IScript::Request& request);
			void RequestLoad(IScript::Request& request, IScript::Delegate<ComputeComponent> computeComponent, String& code);
			void RequestCall(IScript::Request& request, IScript::Delegate<ComputeComponent> computeComponent, IScript::Delegate<ComputeRoutine> computeRoutine, IScript::Request::PlaceHolder ph);
			void RequestCleanup(IScript::Request& request, IScript::Delegate<ComputeComponent> computeComponent);
		};
	}
}


#endif // __COMPUTECOMPONENTMODULE_H__