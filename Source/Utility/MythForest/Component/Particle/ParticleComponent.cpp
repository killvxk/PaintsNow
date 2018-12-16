#include "ParticleComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

ParticleComponent::ParticleComponent() {
}

ParticleComponent::~ParticleComponent() {}

TObject<IReflect>& ParticleComponent::operator () (IReflect& reflect) {
	ReflectClass(ParticleComponent);

	return *this;
}
