// FormComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-4
//

#ifndef __FORMCOMPONENTMODULE_H__
#define __FORMCOMPOENNTMODULE_H__

#include "FormComponent.h"

namespace PaintsNow {
	namespace NsMythForest {
		class FormComponent;
		class FormComponentModule : public ModuleImpl<FormComponent> {
		public:
			FormComponentModule(Engine& engine);
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			void RequestCreate(IScript::Request& request, String& name);
			void RequestResize(IScript::Request& request, IScript::Delegate<FormComponent> formComponent, int32_t index);
			void RequestSetData(IScript::Request& request, IScript::Delegate<FormComponent> formComponent, int32_t index, String& data);
			void RequestGetData(IScript::Request& request, IScript::Delegate<FormComponent> formComponent, int32_t index);
			void RequestGetName(IScript::Request& request, IScript::Delegate<FormComponent> formComponent);
		};
	}
}


#endif // __FORMCOMPONENTMODULE_H__