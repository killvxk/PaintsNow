#include "ShapeComponentModule.h"
#include "../../Engine.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

ShapeComponentModule::ShapeComponentModule(Engine& engine) : ModuleImpl<ShapeComponent>(engine) {}
ShapeComponentModule::~ShapeComponentModule() {}

TObject<IReflect>& ShapeComponentModule::operator () (IReflect& reflect) {
	ReflectClass(ShapeComponentModule);

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestRebuild)[ScriptMethod = "Rebuild"];
	}

	return *this;
}

void ShapeComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(ShapeComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	TShared<ShapeComponent> shapeComponent = TShared<ShapeComponent>::Make();

	request.DoLock();
	request << delegate(shapeComponent());
	request.UnLock();
}

void ShapeComponentModule::RequestRebuild(IScript::Request& request, IScript::Delegate<ShapeComponent> shapeComponent, ZFloat4& color) {
	METHOD_ENTRY(ShapeComponentModule::RequestRebuild);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(shapeComponent);

}