#include "RenderPort.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

TObject<IReflect>& RenderPort::operator () (IReflect& reflect) {
	ReflectClass(RenderPort);

	return *this;
}
