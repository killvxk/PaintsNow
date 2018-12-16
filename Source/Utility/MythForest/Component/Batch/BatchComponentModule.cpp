#include "BatchComponentModule.h"
#include "BatchComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

BatchComponentModule::BatchComponentModule(Engine& engine) : ModuleImpl<BatchComponent>(engine) {}


TObject<IReflect>& BatchComponentModule::operator () (IReflect& reflect) {
	ReflectClass(BatchComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestEnableCapture)[ScriptMethod = "EnableCapture"];
		ReflectMethod(RequestGetCaptureStatistics)[ScriptMethod = "GetCaptureStatistics"];
	}

	return *this;
}

void BatchComponentModule::RequestCreate(IScript::Request& request) {}
void BatchComponentModule::RequestEnableCapture(IScript::Request& request, IScript::Delegate<BatchComponent> component, bool enabled) {}
void BatchComponentModule::RequestGetCaptureStatistics(IScript::Request& request, IScript::Delegate<BatchComponent> component) {}