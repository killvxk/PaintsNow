#include "Controller.h"

using namespace PaintsNow;
using namespace PaintsNow::NsGalaxyWeaver;

Controller::Controller(IThread& threadApi, ITunnel& tunnel, const String& entry) : ProxyStub(threadApi, tunnel, entry) {}
Controller::~Controller() {}

TObject<IReflect>& Controller::operator () (IReflect& reflect) {
	ReflectClass(Controller);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(CheckVersion)[ScriptWrapper];
		ReflectProperty(PostResource)[ScriptWrapper];
		// ReflectProperty(PostCamera)[ScriptWrapper];
	}

	return *this;
}