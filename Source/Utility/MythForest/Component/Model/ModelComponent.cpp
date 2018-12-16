#include "ModelComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

ModelComponent::ModelComponent(TShared<ModelResource> model) : modelResource(model) {}

void ModelComponent::RenderInstance(IRender& render, IRender::Target* renderTarget, Instance& instance) {
	// modelResource->PrepareResource();
}

bool ModelComponent::UpdateRenderData() {
	// get model component data	
	if (modelResource) {
		if (Flag() & MODEL_COMPONENT_OVERRIDE_RENDERDATA) {
			// TODO: copy and apply changes
			renderData = modelResource->renderData;
		}

		return true;
	} else {
		return false;
	}
}