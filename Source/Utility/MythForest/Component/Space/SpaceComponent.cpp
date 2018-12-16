#include "SpaceComponent.h"
#include "../../Entity.h"
#include "../../../BridgeSunset/BridgeSunset.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

SpaceComponent::SpaceComponent() : rootEntity(nullptr) {}
SpaceComponent::~SpaceComponent() {}

TObject<IReflect>& SpaceComponent::operator () (IReflect& reflect) {
	ReflectClass(SpaceComponent)[Interface(Component)];

	return *this;
}

void SpaceComponent::UpdateEntityThreadIndex(Entity* entity) {
	if (entity != nullptr) {
		uint32_t threadIndex = GetWarpIndex();
		entity->SetWarpIndex(threadIndex);
		if (entity->Flag() & Entity::ENTITY_HAS_SPACE) {
			const Entity::ComponentVector& components = entity->GetComponents();

			for (size_t i = 0; i < components.size(); i++) {
				Component* component = components[i];
				if (component->GetEntityMask() & Entity::ENTITY_HAS_SPACE) {
					SpaceComponent* spaceComponent = static_cast<SpaceComponent*>(component);
					if (!(spaceComponent->Flag() & SPACECOMPONENT_LOCALIZED_THREAD_CONTEXT)) {
						spaceComponent->SetWarpIndex(threadIndex);
						spaceComponent->UpdateEntityThreadIndex(spaceComponent->GetRootEntity());
					}
				}
			}
		}

		UpdateEntityThreadIndex(static_cast<Entity*>(entity->Left()));
		UpdateEntityThreadIndex(static_cast<Entity*>(entity->Right()));
	}
}

void SpaceComponent::Initialize(Engine& engine, Entity* entity) {
	if (Flag() & SPACECOMPONENT_LOCALIZED_THREAD_CONTEXT) {
		SetWarpIndex(rand() % engine.GetWarpCount());
	} else {
		Component::Initialize(engine, entity);
	}

	Flag() |= SPACECOMPONENT_ATTACHED;
}

void SpaceComponent::Uninitialize(Engine& engine, Entity* entity) {
	Flag() &= ~SPACECOMPONENT_ATTACHED;

	RemoveAll(engine);
}

void SpaceComponent::QueueRoutine(Engine& engine, ITask* task) {
	engine.QueueRoutine(this, task);
}

void SpaceComponent::QueueQuickRoutine(Engine& engine, ITask* task) {
	if (engine.GetCurrentWarpIndex() == GetWarpIndex()) {
		task->Execute(nullptr);
	} else {
		QueueRoutine(engine, task);
	}
}

bool SpaceComponent::Insert(Engine& engine, Entity* entity) {
	assert(Flag() & SPACECOMPONENT_ATTACHED);

	// already managed by other space, must detach it before calling me.
	if (!(entity->Flag() & Entity::ENTITY_HAS_ENGINE) && entity->GetParent() != nullptr) {
		return false;
	}

	entity->ReferenceObject();
	// cleanup has_engine flag
	entity->CleanupEngineInternal();

	UpdateEntityThreadIndex(entity);

	if (rootEntity) {
		rootEntity->Attach(entity);
	} else {
		rootEntity = entity;
	}

	return true;
}

Entity* SpaceComponent::GetRootEntity() const {
	return rootEntity;
}

void SpaceComponent::SetRootEntity(Entity* root) {
	rootEntity = root;
}

bool SpaceComponent::ValidateEntity(Entity* entity) {
	// Examine parent
	Entity* parent = entity;
	while (parent != nullptr && parent != rootEntity) {
		parent = static_cast<Entity*>(parent->GetParent());
	}

	return parent == rootEntity;
}

bool SpaceComponent::Remove(Engine& engine, Entity* entity) {
	if (!ValidateEntity(entity)) {
		return false;
	}

	Entity* newRoot = static_cast<Entity*>(entity->Detach());
	if (newRoot != nullptr) {
		rootEntity = newRoot;
	}

	entity->SetEngineInternal(engine);
	entity->ReleaseObject();
	return true;
}

void SpaceComponent::RemoveAll(Engine& engine) {
	// Remove all references ...
	Entity* entity = rootEntity;
	FastRemoveNode(engine, entity);
	rootEntity = entity;
}

void SpaceComponent::FastRemoveNode(Engine& engine, Entity*& entity) {
	if (entity != nullptr) {
		FastRemoveNode(engine, entity->Left());
		FastRemoveNode(engine, entity->Right());
		entity->SetEngineInternal(engine); // reset engine
		entity->ReleaseObject();
		entity = nullptr;
	}
}

struct CollectEntityHandler {
public:
	CollectEntityHandler(IScript::Request& r, IScript::Request::Ref cb, uint32_t cc) : request(r), callback(cb), groupCount(cc), currentCount(0) {}

	void Commit() {
		request.Push();
		request << begintable;
		for (size_t i = 0; i < entityList.size(); i++) {
			request << delegate(entityList[i]);
		}
		request << endtable;
		request.Call(Sync, callback);
		request.Pop();
	}

	bool operator () (const Box& box, Tree& tree) {
		if (Overlap(box, tree.GetKey())) {
			entityList.push_back(static_cast<Entity*>(&tree));

			if (groupCount != 0 && (++currentCount % groupCount == 0)) {
				// commit entity list
				request.DoLock();
				Commit();
				request.UnLock();
			}
		}

		return true;
	}

	IScript::Request& request;
	IScript::Request::Ref callback;
	uint32_t groupCount;
	uint32_t currentCount;
	std::vector<Entity*> entityList;
};

void SpaceComponent::RoutineQueryEntities(IScript::Request& request, const ZFloat3Pair& box, IScript::Request::Ref callback, uint32_t groupCount) {
	CollectEntityHandler handler(request, callback, groupCount);
	rootEntity = static_cast<Entity*>(rootEntity->Query(Box(box), handler));

	request.DoLock();

	if (!handler.entityList.empty()) {
		handler.Commit();
	}

	request.Dereference(callback);
	request.UnLock();
}

void SpaceComponent::SetWorldTransform(const ZMatrixFloat4x4& worldTransform) {
	worldTransformCache = ZMatrixFloat3x4(worldTransform);
}

ZMatrixFloat4x4 SpaceComponent::GetWorldTransform() const {
	return ZMatrixFloat4x4(worldTransformCache);
}
