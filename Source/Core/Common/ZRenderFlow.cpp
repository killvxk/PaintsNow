#include "ZRenderFlow.h"
using namespace PaintsNow;

// ZRenderFlow
void ZRenderFlow::InvokeRender(IRender& render) {
	UpdateCacheStages();

	// render work
	for (size_t i = 0; i < cachedRenderStages.size(); i++) {
		std::vector<IRenderStage*>& stages = cachedRenderStages[i];
		// parallelize?
		for (size_t j = 0; j < stages.size(); j++) {
			IRenderStage* p = stages[j];
			// TODO:
		}
	}
}