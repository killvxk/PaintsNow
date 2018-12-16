#include "SpaceComponentModule.h"
#include "../../Entity.h"
#include "../../../BridgeSunset/BridgeSunset.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::NsBridgeSunset;

// Script interfaces
SpaceComponentModule::SpaceComponentModule(Engine& engine) : ModuleImpl<SpaceComponent>(engine) {}
	
TObject<IReflect>& SpaceComponentModule::operator ()(IReflect& reflect) {
	ReflectClass(SpaceComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestInsertEntity)[ScriptMethod = "InsertEntity"];
		ReflectMethod(RequestRemoveEntity)[ScriptMethod = "RemoveEntity"];
		ReflectMethod(RequestQueryEntities)[ScriptMethod = "QueryEntities"];
	}
	return *this;
}

void SpaceComponentModule::RequestCreate(IScript::Request& request) {
	TShared<SpaceComponent> spaceComponent = TShared<SpaceComponent>::Make();
	request.DoLock();
	request << delegate(spaceComponent());
	request.UnLock();
}

void SpaceComponentModule::RequestInsertEntity(IScript::Request& request, IScript::Delegate<SpaceComponent> spaceComponent, IScript::Delegate<Entity> entity) {
	METHOD_ENTRY(SpaceComponentModule::RequestInsertEntity);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(spaceComponent);
	CHECK_DELEGATE(entity);
	CHECK_THREAD(entity);

	if (spaceComponent->Flag() & SpaceComponent::SPACECOMPONENT_ATTACHED) {
		spaceComponent->Insert(engine, entity.Get());
	} else {
		request.Error("Isolated SpaceComponent cannot hold entities.");
	}
}

void SpaceComponentModule::RequestRemoveEntity(IScript::Request& request, IScript::Delegate<SpaceComponent> spaceComponent, IScript::Delegate<Entity> entity) {
	METHOD_ENTRY(SpaceComponentModule::RequestRemoveEntity);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(spaceComponent);
	CHECK_DELEGATE(entity);
	CHECK_THREAD(entity);

	spaceComponent->Remove(engine, entity.Get());
}


void SpaceComponentModule::RequestQueryEntities(IScript::Request& request, IScript::Delegate<SpaceComponent> spaceComponent, ZFloat3Pair& box, IScript::Request::Ref callback, uint32_t groupCount) {
	METHOD_ENTRY(SpaceComponentModule::RequestQueryEntities);
	CHECK_REFERENCES(callback);
	CHECK_DELEGATE(spaceComponent);
	CHECK_THREAD(spaceComponent);

	spaceComponent->RoutineQueryEntities(request, box, callback, groupCount);
}

void SpaceComponentModule::ScriptUninitialize(IScript::Request& request) {
	Module::ScriptUninitialize(request);
}
