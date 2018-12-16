#include "TransformComponentModule.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

TransformComponentModule::TransformComponentModule(Engine& engine) : ModuleImpl<TransformComponent>(engine) {}

TObject<IReflect>& TransformComponentModule::operator () (IReflect& reflect) {
	ReflectClass(TransformComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestLookAt)[ScriptMethod = "LookAt"];

		ReflectMethod(RequestSetTranslation)[ScriptMethod = "SetTranslation"];
		ReflectMethod(RequestGetTranslation)[ScriptMethod = "GetTranslation"];
		ReflectMethod(RequestGetQuickTranslation)[ScriptMethod = "GetQuickTranslation"];
		ReflectMethod(RequestSetRotation)[ScriptMethod = "SetRotation"];
		ReflectMethod(RequestGetRotation)[ScriptMethod = "GetRotation"];
		ReflectMethod(RequestSetScale)[ScriptMethod = "SetScale"];
		ReflectMethod(RequestGetScale)[ScriptMethod = "GetScale"];
	}

	return *this;
}
void TransformComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(TransformComponentModule::RequestCreate);
	TShared<TransformComponent> transformComponent = TShared<TransformComponent>::Make();
	request.DoLock();
	request << delegate(transformComponent());
	request.UnLock();
}

void TransformComponentModule::RequestLookAt(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent, ZFloat3& view, ZFloat3& direction, ZFloat3& up) {
	METHOD_ENTRY(TransformComponentModule::RequestLookAt);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(transformComponent);
	CHECK_THREAD(transformComponent);
	
	transformComponent->LookAt(view, direction, up);
}

void TransformComponentModule::RequestGetQuickTranslation(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent) {
	METHOD_ENTRY(TransformComponentModule::RequestGetQuickTranslation);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(transformComponent);

	request.DoLock();
	request << transformComponent->GetQuickTranslation();
	request.UnLock();
}

void TransformComponentModule::RequestSetRotation(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent, ZFloat3& rotation) {
	METHOD_ENTRY(TransformComponentModule::RequestSetRotation);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(transformComponent);
	CHECK_THREAD(transformComponent);

	transformComponent->SetRotation(rotation);
}

void TransformComponentModule::RequestGetRotation(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent) {
	METHOD_ENTRY(TransformComponentModule::RequestSetRotation);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(transformComponent);
	CHECK_THREAD(transformComponent);

	request.DoLock();
	request << transformComponent->GetRotation();
	request.UnLock();
}

void TransformComponentModule::RequestSetScale(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent, ZFloat3& scale) {
	METHOD_ENTRY(TransformComponentModule::RequestSetScale);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(transformComponent);
	CHECK_THREAD(transformComponent);

	transformComponent->SetScale(scale);
}

void TransformComponentModule::RequestGetScale(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent) {
	METHOD_ENTRY(TransformComponentModule::RequestSetRotation);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(transformComponent);
	CHECK_THREAD(transformComponent);

	request.DoLock();
	request << transformComponent->GetScale();
	request.UnLock();
}

void TransformComponentModule::RequestSetTranslation(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent, ZFloat3& translation) {
	METHOD_ENTRY(TransformComponentModule::RequestSetTranslation);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(transformComponent);
	CHECK_THREAD(transformComponent);

	transformComponent->SetTranslation(translation);
}

void TransformComponentModule::RequestGetTranslation(IScript::Request& request, IScript::Delegate<TransformComponent> transformComponent) {
	METHOD_ENTRY(TransformComponentModule::RequestGetTranslation);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(transformComponent);
	CHECK_THREAD(transformComponent);

	request.DoLock();
	request << transformComponent->GetTranslation();
	request.UnLock();
}
