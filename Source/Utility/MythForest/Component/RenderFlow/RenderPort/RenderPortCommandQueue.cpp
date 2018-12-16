#include "RenderPortCommandQueue.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

TObject<IReflect>& RenderPortCommandQueue::operator () (IReflect& reflect) {
	ReflectClass(RenderPortCommandQueue)[Interface(RenderPort)];

	return *this;
}

IRender::Target* RenderPortCommandQueue::BeginTransaction() {
	// TODO: resizing?
	return renderTargetResource->GetRenderTarget();
}

void RenderPortCommandQueue::EndTransaction() {
	// do nothing
}
