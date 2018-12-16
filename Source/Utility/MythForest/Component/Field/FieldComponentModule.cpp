#include "FieldComponentModule.h"
#include "../../Engine.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

FieldComponentModule::FieldComponentModule(Engine& engine) : ModuleImpl<FieldComponent>(engine) {}
FieldComponentModule::~FieldComponentModule() {}

TObject<IReflect>& FieldComponentModule::operator () (IReflect& reflect) {
	ReflectClass(FieldComponentModule)[Interface(Module)];
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestRebuild)[ScriptMethod = "Rebuild"];
	}

	return *this;
}
void FieldComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(FieldComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	TShared<FieldComponent> fieldComponent = TShared<FieldComponent>::Make();
	request.DoLock();
	request << delegate(fieldComponent());
	request.UnLock();
}

void FieldComponentModule::RequestRebuild(IScript::Request& request, IScript::Delegate<FieldComponent> fieldComponent) {
	METHOD_ENTRY(FieldComponentModule::RequestRebuild);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(fieldComponent);

}