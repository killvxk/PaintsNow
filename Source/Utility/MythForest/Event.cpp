#include "Event.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

Event::Event(Engine& e, EVENT_ID id, TShared<SharedTiny> sender, TShared<SharedTiny> d) : engine(e), eventID(id), eventSender(sender), eventDetail(d) {}
