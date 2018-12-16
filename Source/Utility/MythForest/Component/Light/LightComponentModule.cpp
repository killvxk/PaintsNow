#include "LightComponentModule.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

LightComponentModule::LightComponentModule(Engine& engine) : ModuleImpl<LightComponent>(engine) {}

TObject<IReflect>& LightComponentModule::operator () (IReflect& reflect) {
	ReflectClass(LightComponentModule);

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestSetLightColor)[ScriptMethod = "SetLightColor"];
	}

	return *this;
}

void LightComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(LightComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	TShared<LightComponent> lightComponent = TShared<LightComponent>::Make();

	request.DoLock();
	request << delegate(lightComponent());
	request.UnLock();
}

void LightComponentModule::RequestSetLightColor(IScript::Request& request, IScript::Delegate<LightComponent> lightComponent, ZFloat4& color) {
	METHOD_ENTRY(LightComponentModule::RequestSetLightColor);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(lightComponent);
	CHECK_THREAD(lightComponent);

	lightComponent->color = color;
}