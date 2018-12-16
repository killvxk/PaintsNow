#include "Component.h"
#include "Entity.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

TObject<IReflect>& Component::operator () (IReflect& reflect) {
	ReflectClass(Component)[Interface(Tiny)];

	return *this;
}


void Component::Initialize(Engine& engine, Entity* entity) {
	SetWarpIndex(entity->GetWarpIndex());
}

void Component::Uninitialize(Engine& engine, Entity* entity) {
	
}

void Component::DispatchEvent(Event& event) {}

Tiny::FLAG Component::GetEntityMask() const {
	return 0;
}