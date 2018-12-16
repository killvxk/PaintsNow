#include "SurfaceComponentModule.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

SurfaceComponentModule::SurfaceComponentModule(Engine& engine) : ModuleImpl<SurfaceComponent>(engine) {}


TObject<IReflect>& SurfaceComponentModule::operator () (IReflect& reflect) {
	ReflectClass(SurfaceComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestRebuild)[ScriptMethod = "Rebuild"];
	}

	return *this;
}

void SurfaceComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(SurfaceComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	TShared<SurfaceComponent> surfaceComponent = TShared<SurfaceComponent>::Make();
	request.DoLock();
	request << delegate(surfaceComponent());
	request.UnLock();
}

void SurfaceComponentModule::RequestRebuild(IScript::Request& request, IScript::Delegate<SurfaceComponent> surfaceComponent) {
	METHOD_ENTRY(SurfaceComponentModule::RequestRebuild);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(surfaceComponent);


}