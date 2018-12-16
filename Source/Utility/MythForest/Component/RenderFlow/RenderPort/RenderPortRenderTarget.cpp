#include "RenderPortRenderTarget.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

TObject<IReflect>& RenderPortRenderTarget::operator () (IReflect& reflect) {
	ReflectClass(RenderPortRenderTarget)[Interface(RenderPort)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(renderTargetTextureResource);
	}

	return *this;
}
