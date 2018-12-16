#include "Message.h"

using namespace PaintsNow;
using namespace NsLunaWidget;

RequestState::RequestState() : handled(false) {}

bool RequestState::IsHandled() const {
	return handled;
}

void RequestState::Complete() {
	handled = true;
}
