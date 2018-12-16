#include "Dispatcher.h"
#include "IceSpring.h"

using namespace PaintsNow;
using namespace PaintsNow::NsIceSpring;

Dispatcher::Dispatcher(IceSpring& h, IScript::Request::Ref ref) : host(h), callback(ref) {}

const IScript::Request::Ref& Dispatcher::GetCallback() const {
	return callback;
}

void Dispatcher::ScriptUninitialize(IScript::Request& request) {
	request.DoLock();
	if (callback) {
		request.Dereference(callback);
	}
	request.UnLock();

	host.Remove(this);

	SharedTiny::ScriptUninitialize(request);
}

TObject<IReflect>& Dispatcher::operator () (IReflect& reflect) {
	ReflectClass(Dispatcher);
	if (reflect.IsReflectProperty()) {
		ReflectProperty(host);
		ReflectProperty(callback);
	}

	return *this;
}

// #include <Windows.h>

void Dispatcher::Invoke(IScript::Request& request) {
	// printf("Process message in thread id = %d\n", ::GetCurrentThreadId());
	request.DoLock();
	request.Call(Sync, callback);
	request.UnLock();
}