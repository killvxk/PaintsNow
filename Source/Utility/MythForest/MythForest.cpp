#include "MythForest.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::NsBridgeSunset;

class ModuleRegistar : public IReflect {
public:
	ModuleRegistar(Engine& e) : engine(e), IReflect(true, false) {}
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		if (!s.IsBaseObject() && s.QueryInterface(UniqueType<Module>()) != nullptr) {
			Module& module = static_cast<Module&>(s);
			engine.InstallModule(&module);
		}
	}
	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}

protected:
	Engine& engine;
};

MythForest::MythForest(Interfaces& interfaces, NsSnowyStream::SnowyStream& snowyStream, NsBridgeSunset::BridgeSunset& bridgeSunset, NsFluteLeaves::FluteLeaves& fluteLeaves, size_t warpCount)
	: Library(interfaces.thread), engine(interfaces, bridgeSunset, fluteLeaves, snowyStream, *this, warpCount),
	animationComponentModule(engine),
	batchComponentModule(engine),
	eventListenerComponentModule(engine),
	cameraComponentModule(engine),
	computeComponentModule(engine),
	fieldComponentModule(engine),
	formComponentModule(engine),
	modelComponentModule(engine),
	lightComponentModule(engine),
	particleComponentModule(engine),
	renderFlowComponentModule(engine),
	shapeComponentModule(engine),
	soundComponentModule(engine),
	spaceComponentModule(engine),
	surfaceComponentModule(engine),
	terrainComponentModule(engine),
	transformComponentModule(engine) {
	ModuleRegistar registar(engine);
	(*this)(registar);
}

MythForest::~MythForest() {
	engine.Clear();
}

TObject<IReflect>& MythForest::operator () (IReflect& reflect) {
	ReflectClass(MythForest)[Interface(IScript::Library)];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(engine)[Runtime];
		ReflectProperty(globalEntity)[Runtime];
	
		ReflectProperty(animationComponentModule)[ScriptLibrary = "AnimationComponentModule"];
		ReflectProperty(batchComponentModule)[ScriptLibrary = "BatchComponentModule"];
		ReflectProperty(cameraComponentModule)[ScriptLibrary = "CameraComponentModule"];
		ReflectProperty(computeComponentModule)[ScriptLibrary = "ComputeComponentModule"];
		ReflectProperty(eventListenerComponentModule)[ScriptLibrary = "EventListenerComponentModule"];
		ReflectProperty(fieldComponentModule)[ScriptLibrary = "FieldComponentModule"];
		ReflectProperty(formComponentModule)[ScriptLibrary = "FormComponentModule"];
		ReflectProperty(lightComponentModule)[ScriptLibrary = "LightComponentModule"];
		ReflectProperty(modelComponentModule)[ScriptLibrary = "ModelComponentModule"];
		ReflectProperty(particleComponentModule)[ScriptLibrary = "ParticleComponentModule"];
		ReflectProperty(renderFlowComponentModule)[ScriptLibrary = "RenderFlowComponentModule"];
		ReflectProperty(soundComponentModule)[ScriptLibrary = "SoundComponentModule"];
		ReflectProperty(spaceComponentModule)[ScriptLibrary = "SpaceComponentModule"];
		ReflectProperty(surfaceComponentModule)[ScriptLibrary = "SurfaceComponentModule"];
		ReflectProperty(terrainComponentModule)[ScriptLibrary = "TerrainComponentModule"];
		ReflectProperty(transformComponentModule)[ScriptLibrary = "TransformComponentModule"];
	}

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestEnumerateComponentModules)[ScriptMethod = "EnumerateComponentModules"];
		ReflectMethod(RequestCreateEntity)[ScriptMethod = "CreateEntity"];
		ReflectMethod(RequestQueueRoutine)[ScriptMethod = "QueueRoutine"];
		ReflectMethod(RequestUpdateEntity)[ScriptMethod = "UpdateEntity"];
		ReflectMethod(RequestAddEntityComponent)[ScriptMethod = "AddEntityComponent"];
		ReflectMethod(RequestRemoveEntityComponent)[ScriptMethod = "RemoveEntityComponent"];
		ReflectMethod(RequestGetEntityComponents)[ScriptMethod = "GetEntityComponents"];
		ReflectMethod(RequestClearEntityComponents)[ScriptMethod = "ClearEntityComponents"];
	}

	return *this;
}

void MythForest::OnRender() {
	engine.TickFrame();
}

void MythForest::RequestCreateEntity(IScript::Request& request) {
	METHOD_ENTRY(MythForest::RequestCreateEntity);

	TShared<Entity> entity = TShared<Entity>::Make();
	uint32_t warpIndex = engine.GetCurrentWarpIndex();
	entity->SetWarpIndex(warpIndex);
	entity->SetEngineInternal(engine);

	request.DoLock();
	request << delegate(entity());
	request.UnLock();
}

void MythForest::RequestEnumerateComponentModules(IScript::Request& request) {
	METHOD_ENTRY(MythForest::RequestEnumerateComponentModules);
	request.DoLock();
	request << begintable;
	const unordered_map<String, Module*>& subModules = engine.GetModuleMap();
	for (unordered_map<String, Module*>::const_iterator it = subModules.begin(); it != subModules.end(); ++it) {
		request << key((*it).first) << *((*it).second);
	}

	request << endtable;
	request.UnLock();
}

void MythForest::RequestAddEntityComponent(IScript::Request& request, IScript::Delegate<Entity> entity, IScript::Delegate<Component> component) {
	METHOD_ENTRY(MythForest::RequestAddEntityComponent);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(entity);
	CHECK_DELEGATE(component);
	CHECK_THREAD(entity);

	entity->AddComponent(engine, component.Get());
}

void MythForest::RequestUpdateEntity(IScript::Request& request, IScript::Delegate<Entity> entity) {
	METHOD_ENTRY(MythForest::RequestUpdateEntity);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(entity);
	CHECK_THREAD(entity);

	entity->UpdateEntityFlags();
}

void MythForest::RequestRemoveEntityComponent(IScript::Request& request, IScript::Delegate<Entity> entity, IScript::Delegate<Component> component) {
	METHOD_ENTRY(MythForest::RequestRemoveEntityComponent);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(entity);
	CHECK_DELEGATE(component);
	CHECK_THREAD(entity);

	entity->RemoveComponent(engine, component.Get());
}

void MythForest::RequestGetUniqueEntityComponent(IScript::Request& request, IScript::Delegate<Entity> entity, String& componentName) {
	METHOD_ENTRY(MythForest::RequestGetUniqueEntityComponent);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(entity);
	CHECK_THREAD(entity);

	Module* module = engine.GetComponentModuleFromName(componentName);
	if (module != nullptr) {
		Component* component = entity->GetUniqueComponent(module->GetTinyUnique());
		if (component != nullptr) {
			request.DoLock();
			request << delegate(component);
			request.UnLock();
		}
		component->ReleaseObject();
	}
}

void MythForest::RequestGetEntityComponents(IScript::Request& request, IScript::Delegate<Entity> entity, String& componentName) {
	METHOD_ENTRY(MythForest::RequestGetEntityComponents);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(entity);
	CHECK_THREAD(entity);

	Module* module = engine.GetComponentModuleFromName(componentName);
	if (module != nullptr) {
		Unique unique = module->GetTinyUnique();
		const Entity::ComponentVector& components = entity->GetComponents();
		request.DoLock();
		request << begintable;

		for (size_t i = 0; i < components.size(); i++) {
			Component* component = components[i];
			if (componentName.empty() || component->GetUnique() == unique) {
				request << delegate(component);
			}
		}

		request << endtable;
		request.UnLock();
	}
}

void MythForest::RequestClearEntityComponents(IScript::Request& request, IScript::Delegate<Entity> entity) {
	METHOD_ENTRY(MythForest::RequestClearEntityComponents);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(entity);
	CHECK_THREAD(entity);

	entity->ClearComponents(engine);
}

void MythForest::RequestQueueRoutine(IScript::Request& request, IScript::Delegate<Unit> unit, IScript::Request::Ref callback) {
	METHOD_ENTRY(MythForest::RequestQueueRoutine);
	CHECK_REFERENCES_WITH_TYPE(callback, IScript::Request::FUNCTION);
	CHECK_DELEGATE(unit);

	engine.QueueRoutine(unit.Get(), WrapTaskScript(callback));
	request.DoLock();
	request.Dereference(callback);
	request.UnLock();
}