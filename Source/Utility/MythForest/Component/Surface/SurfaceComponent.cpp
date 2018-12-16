#include "SurfaceComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

SurfaceComponent::SurfaceComponent() {}

bool SurfaceComponent::UpdateRenderData() {
	return false;
}

void SurfaceComponent::RenderInstance(IRender& render, IRender::Target* renderTarget, Instance& instance) {}
