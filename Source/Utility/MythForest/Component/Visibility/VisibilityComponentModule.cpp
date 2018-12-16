#include "VisibilityComponentModule.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

VisibilityComponentModule::VisibilityComponentModule(Engine& engine) : ModuleImpl<VisibilityComponent>(engine) {}

TObject<IReflect>& VisibilityComponentModule::operator () (IReflect& reflect) {
	ReflectClass(VisibilityComponentModule);

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestRebuild)[ScriptMethod = "Rebuild"];
	}

	return *this;
}

void VisibilityComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(VisibilityComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	TShared<VisibilityComponent> visibilityComponent = TShared<VisibilityComponent>::Make();

	request.DoLock();
	request << delegate(visibilityComponent());
	request.UnLock();
}

void VisibilityComponentModule::RequestRebuild(IScript::Request& request, IScript::Delegate<VisibilityComponent> visibilityComponent) {
	METHOD_ENTRY(VisibilityComponentModule::RequestRebuild);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(visibilityComponent);

}


