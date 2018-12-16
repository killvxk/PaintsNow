#include "FieldComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

FieldComponent::FieldComponent() {
}

FieldComponent::~FieldComponent() {}

TObject<IReflect>& FieldComponent::operator () (IReflect& reflect) {
	ReflectClass(FieldComponent);

	return *this;
}
