#include "RenderPortLightSource.h"
#include "../../Light/LightComponent.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

TObject<IReflect>& RenderPortLightSource::operator () (IReflect& reflect) {
	ReflectClass(RenderPortLightSource)[Interface(RenderPort)];

	return *this;
}

void RenderPortLightSource::UpdateLight(std::vector<LightElement>& lightElements) {
	this->lightElements = lightElements;
}