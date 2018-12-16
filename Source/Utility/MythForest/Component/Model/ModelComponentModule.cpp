#include "ModelComponentModule.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

ModelComponentModule::ModelComponentModule(Engine& engine) : ModuleImpl<ModelComponent>(engine) {}

TObject<IReflect>& ModelComponentModule::operator () (IReflect& reflect) {
	ReflectClass(ModelComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestRebuild)[ScriptMethod = "Rebuild"];
	}

	return *this;
}

void ModelComponentModule::RequestCreate(IScript::Request& request, IScript::Delegate<ModelResource> modelResource) {
	METHOD_ENTRY(ModelComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(modelResource);

	TShared<ModelResource> res = modelResource.Get();
	TShared<ModelComponent> modelComponent = TShared<ModelComponent>::Make(res);
	request.DoLock();
	request << delegate(modelComponent());
	request.UnLock();
}

void ModelComponentModule::RequestRebuild(IScript::Request& request, IScript::Delegate<ModelComponent> modelComponent) {
	METHOD_ENTRY(ModelComponentModule::RequestRebuild);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(modelComponent);

}