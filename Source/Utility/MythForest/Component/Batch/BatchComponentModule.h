// BatchComponentModule.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __BATCHCOMPONENTMODULE_H__
#define __BATCHCOMPONENTMODULE_H__

#include "BatchComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class BatchComponentModule : public ModuleImpl<BatchComponent> {
		public:
			BatchComponentModule(Engine& engine);

			virtual TObject<IReflect>& operator () (IReflect& reflect);

		public:
			// APIs
			void RequestCreate(IScript::Request& request);
			void RequestEnableCapture(IScript::Request& request, IScript::Delegate<BatchComponent> component, bool enabled);
			void RequestGetCaptureStatistics(IScript::Request& request, IScript::Delegate<BatchComponent> component);
		};
	}
}


#endif // __BATCHCOMPONENTMODULE_H__
