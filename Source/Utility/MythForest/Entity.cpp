#include "Entity.h"
#include "Component.h"
#include "Engine.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

Entity::Entity() {}

Entity::~Entity() {
	if (Flag() & ENTITY_HAS_ENGINE) {
		Engine& engine = GetEngineInternal();
		ClearComponents(engine);
	} else {
		assert(components.empty());
	}
}

void Entity::SetEngineInternal(Engine& engine) {
	// reuse parent node as engine pointer
	assert(parentNode == nullptr);
	assert(!(Flag() & ENTITY_HAS_ENGINE));

	Flag() |= ENTITY_HAS_ENGINE;
	parentNode = reinterpret_cast<Tree::Type*>(&engine);
}

void Entity::CleanupEngineInternal() {
	Flag() &= ~ENTITY_HAS_ENGINE;
	parentNode = nullptr;
}

Engine& Entity::GetEngineInternal() const {
	assert(parentNode != nullptr);
	assert(Flag() & ENTITY_HAS_ENGINE);

	return *reinterpret_cast<Engine*>(parentNode);
}

void Entity::AddComponent(Engine& engine, Component* component) {
	// is component unique?
	if (component->Flag() & Tiny::TINY_UNIQUE) {
		Unique unique = component->GetUnique();
		for (size_t i = 0; i < components.size(); i++) {
			Component*& c = components[i];
			if (c == component) return;
			if (c->GetUnique() == unique) {
				component->ReferenceObject();
				component->Initialize(engine, this);
				std::swap(c, component);
				component->ReleaseObject();
				component->Uninitialize(engine, this);
				return;
			}
		}
	}

	// new one
	components.push_back(component);
	component->ReferenceObject();
	component->Initialize(engine, this);

	// add component mask
	Flag() |= component->GetEntityMask();

	if (Flag() & ENTITY_HAS_TACH_EVENTS) {
		Event event(engine, Event::EVENT_ATTACH_COMPONENT, this, component);
		PostEvent(event);
	}
}

void Entity::RemoveComponent(Engine& engine, Component* component) {
	for (ComponentVector::iterator p = components.begin(); p != components.end(); ++p) {
		if (*p == component) {
			components.erase(p);
			component->ReleaseObject();
			component->Uninitialize(engine, this);

			if (Flag() & ENTITY_HAS_TACH_EVENTS) {
				Event event(engine, Event::EVENT_DETACH_COMPONENT, this, component);
				PostEvent(event);
			}

			break;
		}
	}
}

void Entity::UpdateEntityFlags() {
	FLAG p = 0, m;
	FLAG f = 0;
	for (size_t i = 0; i < components.size(); i++) {
		f |= components[i]->GetEntityMask();
	}

	do {
		p = Flag().load(std::memory_order_acquire);
		m = (p & ~ENTITY_HAS_ALL) | f;
	} while (!flag.compare_exchange_weak(p, m, std::memory_order_release));
}

const Entity::ComponentVector& Entity::GetComponents() const {
	return components;
}

Component* Entity::GetUniqueComponent(Unique unique) const {
	for (size_t i = 0; i < components.size(); i++) {
		Component* component = components[i];
		if (component->GetUnique() == unique) {
			assert(component->Flag() & Tiny::TINY_UNIQUE);
			return component;
		}
	}

	return nullptr;
}

void Entity::ClearComponents(Engine& engine) {
	if (Flag() & ENTITY_HAS_TACH_EVENTS) {
		for (ComponentVector::iterator p = components.begin(); p != components.end(); ++p) {
			Event event(engine, Event::EVENT_DETACH_COMPONENT, this, *p);
			PostEvent(event);
		}
	}

	ComponentVector temp;
	std::swap(temp, components);

	for (ComponentVector::iterator p = temp.begin(); p != temp.end(); ++p) {
		(*p)->Uninitialize(engine, this);
		(*p)->ReleaseObject();
	}
}

void Entity::PostEvent(Event& event) {
	for (ComponentVector::iterator p = components.begin(); p != components.end(); ++p) {
		(*p)->DispatchEvent(event);
	}
}

TObject<IReflect>& Entity::operator () (IReflect& reflect) {
	ReflectClass(Entity)[Interface(Unit)];
	// Warning: Force casting, safe on memory layout.
	Vector<IReflectObjectComplex*>& components = reinterpret_cast<Vector<IReflectObjectComplex*>&>(this->components);

	if (reflect.IsReflectProperty()) {
		ReflectProperty(IterateVector(components));
	}

	return *this;
}

// https://github.com/gszauer/GamePhysicsCookbook/blob/master/Code/Geometry3D.cpp
#if defined(_MSC_VER) && _MSC_VER <= 1200
#define fminf Min
#define fmaxf Max
#endif

inline bool Intersect(const Box& aabb, const ZFloat3Pair& ray) {
	const ZFloat3& minValue = aabb.first;
	const ZFloat3& maxValue = aabb.second;

	float dx = ray.second.x();
	float dy = ray.second.y();
	float dz = ray.second.z();
	float dxdy = dx * dy;
	float dydz = dy * dz;
	float dxdz = dx * dz;
	float sign = dxdy * dz;

	float t1 = (minValue.x() - ray.first.x());
	float t2 = (maxValue.x() - ray.first.x());
	float t3 = (minValue.y() - ray.first.y());
	float t4 = (maxValue.y() - ray.first.y());
	float t5 = (minValue.z() - ray.first.z());
	float t6 = (maxValue.z() - ray.first.z());

	float tmin = fmaxf(fmaxf(fminf(t1, t2) * dydz, fminf(t3, t4) * dxdz), fminf(t5, t6) * dxdy);
	float tmax = fminf(fminf(fmaxf(t1, t2) * dydz, fmaxf(t3, t4) * dxdz), fmaxf(t5, t6) * dxdy);

	return (sign >= 0 ? tmin >= 0 : tmax <= 0) && tmin <= tmax;
}


Unit* Entity::Raycast(ZFloat3& intersection, ZFloat3& normal, const ZFloat3Pair& ray, IReflectObject* metaInfo) const {
	if (!Intersect(key, ray)) return nullptr;

	for (size_t i = 0; i < components.size(); i++) {
		Component* component = components[i];
		if (component->GetEntityMask() & ENTITY_HAS_SPACE) {
			Unit* result = component->Raycast(intersection, normal, ray, metaInfo);
			if (result != nullptr) {
				return result;
			}
		}
	}

	return nullptr;
}