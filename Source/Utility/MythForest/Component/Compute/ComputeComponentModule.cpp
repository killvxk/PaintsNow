#include "ComputeComponentModule.h"
#include "../../Engine.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

ComputeComponentModule::ComputeComponentModule(Engine& engine) : ModuleImpl<ComputeComponent>(engine) {}
ComputeComponentModule::~ComputeComponentModule() {}

TObject<IReflect>& ComputeComponentModule::operator () (IReflect& reflect) {
	ReflectClass(ComputeComponentModule)[Interface(Module)];
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestLoad)[ScriptMethod = "Load"];
		ReflectMethod(RequestCall)[ScriptMethod = "Call"];
		ReflectMethod(RequestCleanup)[ScriptMethod = "Cleanup"];
	}

	return *this;
}

void ComputeComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(ComputeComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	IScript* script = engine.interfaces.script.NewScript();
	if (script != nullptr) {
		TShared<ComputeComponent> computeComponent = TShared<ComputeComponent>::Make(engine, script, *request.GetScript());
		
		request.DoLock();
		request << delegate(computeComponent());
		request.UnLock();
	}
}

void ComputeComponentModule::RequestLoad(IScript::Request& request, IScript::Delegate<ComputeComponent> computeComponent, String& code) {
	METHOD_ENTRY(ComputeComponentModule::RequestLoad);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(computeComponent);
	CHECK_THREAD(computeComponent);

	TShared<ComputeRoutine> routine = computeComponent->Load(code);
	if (routine) {
		request.DoLock();
		request << delegate(routine());
		request.UnLock();
	}
}

void ComputeComponentModule::RequestCall(IScript::Request& request, IScript::Delegate<ComputeComponent> computeComponent, IScript::Delegate<ComputeRoutine> computeRoutine, IScript::Request::PlaceHolder ph) {
	METHOD_ENTRY(ComputeComponentModule::RequestCall);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(computeComponent);
	CHECK_DELEGATE(computeRoutine);
	CHECK_THREAD(computeComponent);

	computeComponent->Call(request, computeRoutine.Get());
}

void ComputeComponentModule::RequestCleanup(IScript::Request& request, IScript::Delegate<ComputeComponent> computeComponent) {
	METHOD_ENTRY(ComputeComponentModule::RequestCleanup);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(computeComponent);
	CHECK_THREAD(computeComponent);

	computeComponent->Cleanup();
}