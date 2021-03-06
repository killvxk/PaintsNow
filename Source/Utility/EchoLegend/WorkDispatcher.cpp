#include "WorkDispatcher.h"

using namespace PaintsNow;
using namespace PaintsNow::NsEchoLegend;

WorkDispatcher::WorkDispatcher(NsBridgeSunset::BridgeSunset& bridgeSunset, INetwork& network, ITunnel::Dispatcher* disp) : Looper(bridgeSunset, network), dispatcher(disp) {}

ITunnel::Dispatcher* WorkDispatcher::GetDispatcher() const {
	return dispatcher;
}

TObject<IReflect>& WorkDispatcher::operator () (IReflect& reflect) {
	ReflectClass(WorkDispatcher);
	return *this;
}

bool WorkDispatcher::Activate() {
	return network.ActivateDispatcher(dispatcher);
}
void WorkDispatcher::Deactivate() {
	network.DeactivateDispatcher(dispatcher);
}