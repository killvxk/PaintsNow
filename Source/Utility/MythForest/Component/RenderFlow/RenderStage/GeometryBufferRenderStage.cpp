#include "GeometryBufferRenderStage.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

GeometryBufferRenderStage::GeometryBufferRenderStage() {}

TObject<IReflect>& GeometryBufferRenderStage::operator () (IReflect& reflect) {
	ReflectClass(GeometryBufferRenderStage)[Interface(RenderStage)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(Primitives);
		ReflectProperty(BaseColor);
		ReflectProperty(NormalDepth);
		ReflectProperty(Material);
	}

	return *this;
}
