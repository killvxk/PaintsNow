#include "DeferredLightingRenderStage.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

DeferredLightingRenderStage::DeferredLightingRenderStage() {}

TObject<IReflect>& DeferredLightingRenderStage::operator () (IReflect& reflect) {
	ReflectClass(DeferredLightingRenderStage)[Interface(RenderStage)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(LightSource);

		ReflectProperty(BaseColor);
		ReflectProperty(NormalDepth);
		ReflectProperty(Material);
	}

	return *this;
}
