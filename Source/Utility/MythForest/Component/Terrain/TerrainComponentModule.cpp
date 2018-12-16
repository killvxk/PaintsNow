#include "TerrainComponentModule.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

TerrainComponentModule::TerrainComponentModule(Engine& engine) : ModuleImpl<TerrainComponent>(engine) {}

TObject<IReflect>& TerrainComponentModule::operator () (IReflect& reflect) {
	ReflectClass(TerrainComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestRebuild)[ScriptMethod = "Rebuild"];
	}

	return *this;
}

void TerrainComponentModule::RequestCreate(IScript::Request& request, IScript::Delegate<TerrainResource> terrainResource) {
	METHOD_ENTRY(TerrainComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(terrainResource);

	TShared<TerrainComponent> terrainComponent = TShared<TerrainComponent>::Make();
	request.DoLock();
	request << delegate(terrainComponent());
	request.UnLock();
}

void TerrainComponentModule::RequestRebuild(IScript::Request& request, IScript::Delegate<TerrainComponent> terrainComponent) {
	METHOD_ENTRY(TerrainComponentModule::RequestRebuild);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(terrainComponent);


}