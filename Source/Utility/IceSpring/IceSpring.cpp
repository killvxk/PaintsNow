#include "IceSpring.h"
#include "Dispatcher.h"

using namespace PaintsNow;
using namespace PaintsNow::NsIceSpring;
using namespace PaintsNow::NsBridgeSunset;

IceSpring::IceSpring(IThread& thread, BridgeSunset& bridge) : Library(thread), bridgeSunset(bridge) {}

TObject<IReflect>& IceSpring::operator () (IReflect& reflect) {
	ReflectClass(IceSpring)[Interface(IScript::Library)];
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreateDispatch)[ScriptMethod = "CreateDispatch"];
		ReflectMethod(RequestPost)[ScriptMethod = "Post"];
	}

	return *this;
}

void IceSpring::RequestCreateDispatch(IScript::Request& request, String& type, IScript::Request::Ref ref) {
	METHOD_ENTRY(IceSpring::RequestCreateDispatch);
	CHECK_REFERENCES_WITH_TYPE(ref, IScript::Request::FUNCTION);

	DoLock();
	TShared<Dispatcher> dispatcher = TShared<Dispatcher>::Make(*this, ref);
	if (type == "Mouse") {
		mouseRoutines.insert(dispatcher());
	} else if (type == "Keyboard") {
		keyboardRoutines.insert(dispatcher());
	} else if (type == "Resize") {
		resizeRoutines.insert(dispatcher());
	} else if (type == "Post") {
		postRoutines.insert(dispatcher());
	}

	UnLock();

	request.DoLock();
	request << delegate(dispatcher());
	request.UnLock();
}

// #include <Windows.h>

void IceSpring::ForwardMessage(IScript::Request* request, Dispatcher* disp, bool realtimeTask) {
	// printf("Send message in thread id = %d\n", ::GetCurrentThreadId());
	// TODO:
	// bridgeSunset.Dispatch(request, Wrap(disp, &Dispatcher::Invoke), realtimeTask);
}

void IceSpring::OnMouse(IScript::Request& request, const IFrame::EventMouse& mouse) {
	DoLock();
	for (std::set<Dispatcher*>::const_iterator it = mouseRoutines.begin(); it != mouseRoutines.end(); ) {
		std::set<Dispatcher*>::const_iterator p = it++;
		bridgeSunset.Dispatch(WrapTaskScript((*p)->GetCallback(), (bool)mouse.down, (bool)mouse.left, (bool)mouse.move, (bool)mouse.wheel, (float)mouse.position.x(), (float)mouse.position.y()));
	}

	UnLock();
}

void IceSpring::OnKeyboard(IScript::Request& request, const IFrame::EventKeyboard& keyboard) {
	DoLock();
	for (std::set<Dispatcher*>::const_iterator it = keyboardRoutines.begin(); it != keyboardRoutines.end(); ) {
		std::set<Dispatcher*>::const_iterator p = it++;
		bridgeSunset.Dispatch(WrapTaskScript((*p)->GetCallback(), keyboard.keyCode, String(keyboard.GetName()), ((keyboard.keyCode & IFrame::EventKeyboard::KEY_POP) == 0)));
	}

	UnLock();
}

void IceSpring::OnResize(IScript::Request& request, const ZInt2& newSize) {
	DoLock();
	for (std::set<Dispatcher*>::const_iterator it = resizeRoutines.begin(); it != resizeRoutines.end(); ) {
		std::set<Dispatcher*>::const_iterator p = it++;
		bridgeSunset.Dispatch(WrapTaskScript((*p)->GetCallback(), ZInt2(newSize)));
	}
	UnLock();
}

void IceSpring::Remove(Dispatcher* dispatcher) {
	DoLock();
	mouseRoutines.erase(dispatcher);
	keyboardRoutines.erase(dispatcher);
	resizeRoutines.erase(dispatcher);
	postRoutines.erase(dispatcher);
	UnLock();
}

void IceSpring::RequestPost(IScript::Request& request, IScript::Request::Ref ref, bool realtimeTask) {
	METHOD_ENTRY(IceSpring::RequestPost);
	CHECK_REFERENCES_WITH_TYPE(ref, IScript::Request::FUNCTION);

	DoLock();
	for (std::set<Dispatcher*>::const_iterator it = postRoutines.begin(); it != postRoutines.end();) {
		std::set<Dispatcher*>::const_iterator p = it++;
		bridgeSunset.Dispatch(WrapTaskScript((*p)->GetCallback(), IScript::Request::Ref(ref)));
	}

	if (ref) {
		request.DoLock();
		request.Dereference(ref);
		request.UnLock();
	}

	UnLock();
}
