#include "VisibilityComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

VisibilityComponent::VisibilityComponent() {
}

Tiny::FLAG VisibilityComponent::GetEntityMask() const {
	return Entity::ENTITY_HAS_LIGHT;
}

TObject<IReflect>& VisibilityComponent::operator () (IReflect& reflect) {
	ReflectClass(VisibilityComponent);

	return *this;
}