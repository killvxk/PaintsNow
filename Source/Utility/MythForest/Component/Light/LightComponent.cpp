#include "LightComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

LightComponent::LightComponent() {
}

Tiny::FLAG LightComponent::GetEntityMask() const {
	return Entity::ENTITY_HAS_LIGHT;
}

TObject<IReflect>& LightComponent::operator () (IReflect& reflect) {
	ReflectClass(LightComponent);

	if (reflect.IsReflectProperty()) {
		ReflectProperty(color);
	}

	return *this;
}