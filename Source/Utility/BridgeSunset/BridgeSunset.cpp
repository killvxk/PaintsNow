#include "BridgeSunset.h"

using namespace PaintsNow;
using namespace PaintsNow::NsBridgeSunset;


BridgeSunset::BridgeSunset(IThread& t, IScript& s, size_t threadCount) : IScript::Library(t), script(s), threadPool(t, threadCount, true) {
	BindRequests();
}

BridgeSunset::~BridgeSunset() {
	Reset(false);
}

void BridgeSunset::BindRequests() {
	IScript::Request& request = script.GetDefaultRequest();
	request.DoLock();
	for (size_t i = 0; i < threadPool.GetThreadCount(); i++) {
		assert(threadPool.GetThreadContext(i) == nullptr);
		threadPool.SetThreadContext(i, script.NewRequest());
	}
	request.UnLock();
}

void BridgeSunset::UnbindRequests() {
	IScript::Request& request = script.GetDefaultRequest();
	request.DoLock();
	for (size_t i = 0; i < threadPool.GetThreadCount(); i++) {
		IScript::Request* request = reinterpret_cast<IScript::Request*>(threadPool.GetThreadContext(i));
		if (request != nullptr) {
			assert(request != nullptr);
			request->ReleaseObject();
		}
	}
	request.UnLock();
}

void BridgeSunset::Reset(bool reload) {
	UnbindRequests();
	threadPool.Reset(reload);
	if (reload) {
		BindRequests();
	}
}

void BridgeSunset::ScriptUninitialize(IScript::Request& request) {
	Reset(false);
	Library::ScriptUninitialize(request);
}

TObject<IReflect>& BridgeSunset::operator () (IReflect& reflect) {
	ReflectClass(BridgeSunset)[Interface(IScript::Library)];

	return *this;
}

void BridgeSunset::Require(IScript::Request& request) {
	Library::Require(request);
}

IScript& BridgeSunset::GetScript() {
	return script;
}

ZThreadPool& BridgeSunset::GetThreadPool() {
	return threadPool;
}

void BridgeSunset::Dispatch(ITask* task) {
	threadPool.Push(task);
}
