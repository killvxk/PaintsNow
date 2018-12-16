#include "FormComponentModule.h"
#include "FormComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

FormComponentModule::FormComponentModule(Engine& engine) : ModuleImpl<FormComponent>(engine) {}

TObject<IReflect>& FormComponentModule::operator () (IReflect& reflect) {
	ReflectClass(FormComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestGetData)[ScriptMethod = "GetData"];
		ReflectMethod(RequestGetName)[ScriptMethod = "GetName"];
		ReflectMethod(RequestResize)[ScriptMethod = "Resize"];
		ReflectMethod(RequestSetData)[ScriptMethod = "SetData"];
	}

	return *this;
}

void FormComponentModule::RequestCreate(IScript::Request& request, String& name) {
	METHOD_ENTRY(FormComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	TShared<FormComponent> formComponent = TShared<FormComponent>::Make(name);
	request.DoLock();
	request << delegate(formComponent());
	request.UnLock();
}

void FormComponentModule::RequestResize(IScript::Request& request, IScript::Delegate<FormComponent> formComponent, int32_t index) {
	METHOD_ENTRY(FormComponentModule::RequestResize);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(formComponent);
	CHECK_THREAD(formComponent);

	if (index >= 0) {
		formComponent->values.resize((size_t)index);
	}
}

void FormComponentModule::RequestSetData(IScript::Request& request, IScript::Delegate<FormComponent> formComponent, int32_t index, String& data) {
	METHOD_ENTRY(FormComponentModule::RequestSetData);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(formComponent);
	CHECK_THREAD(formComponent);

	if (index >= 0 && index < (int64_t)formComponent->values.size()) {
		formComponent->values[(size_t)index] = std::move(data);
	}
}

void FormComponentModule::RequestGetData(IScript::Request& request, IScript::Delegate<FormComponent> formComponent, int32_t index) {
	METHOD_ENTRY(FormComponentModule::RequestGetData);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(formComponent);
	CHECK_THREAD(formComponent);

	if (index >= 0 && index < (int32_t)formComponent->values.size()) {
		const String& v = formComponent->values[(size_t)index];
		request.DoLock();
		request << v;
		request.UnLock();
	}
}

void FormComponentModule::RequestGetName(IScript::Request& request, IScript::Delegate<FormComponent> formComponent) {
	METHOD_ENTRY(FormComponentModule::RequestGetName);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(formComponent);
	CHECK_THREAD(formComponent);

	request.DoLock();
	request << formComponent->name;
	request.UnLock();
}