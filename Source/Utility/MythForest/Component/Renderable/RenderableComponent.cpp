#include "RenderableComponent.h"
#include "../../Entity.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

RenderableComponent::RenderableComponent()  {}

Tiny::FLAG RenderableComponent::GetEntityMask() const {
	return Entity::ENTITY_HAS_RENDERABLE;
}

bool RenderableComponent::UpdateCommandCapture(ZBatchRanger& batchRanger, ZBatchRanger::CommandCapture& commandCapture) {
	return false;
}