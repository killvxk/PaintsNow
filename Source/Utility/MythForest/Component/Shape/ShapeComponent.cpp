#include "ShapeComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

ShapeComponent::ShapeComponent() {
}

ShapeComponent::~ShapeComponent() {}

TObject<IReflect>& ShapeComponent::operator () (IReflect& reflect) {
	ReflectClass(ShapeComponent);

	return *this;
}
