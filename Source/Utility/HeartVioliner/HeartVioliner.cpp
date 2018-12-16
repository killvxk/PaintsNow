#include "HeartVioliner.h"
#include "Queue.h"
#include "Clock.h"

using namespace PaintsNow;
using namespace PaintsNow::NsHeartVioliner;
using namespace PaintsNow::NsBridgeSunset;


HeartVioliner::HeartVioliner(IThread& thread, ITimer& base, BridgeSunset& b) : Library(thread), timerFactory(base), bridgeSunset(b) {}

TObject<IReflect>& HeartVioliner::operator () (IReflect& reflect) {
	ReflectClass(HeartVioliner)[Interface(IScript::Library)];
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreateClock)[ScriptMethod = "CreateClock"];
		ReflectMethod(RequestSetClock)[ScriptMethod = "SetClock"];
		ReflectMethod(RequestAttach)[ScriptMethod = "Attach"];
		ReflectMethod(RequestDetach)[ScriptMethod = "Detach"];
		ReflectMethod(RequestStart)[ScriptMethod = "Start"];
		ReflectMethod(RequestPause)[ScriptMethod = "Pause"];
		ReflectMethod(RequestNow)[ScriptMethod = "Now"];
		ReflectMethod(RequestCreateQueue)[ScriptMethod = "CreateQueue"];
		ReflectMethod(RequestListen)[ScriptMethod = "Listen"];
		ReflectMethod(RequestPush)[ScriptMethod = "Push"];
		ReflectMethod(RequestPop)[ScriptMethod = "Pop"];
		ReflectMethod(RequestClear)[ScriptMethod = "Clear"];
	}

	return *this;
}


void HeartVioliner::RequestCreateQueue(IScript::Request& request) {
	METHOD_ENTRY(HeartVioliner::RequestCreateQueue);
	TShared<Queue> q = TShared<Queue>::Make();
	request.DoLock();
	request << delegate(q());
	request.UnLock();
}

void HeartVioliner::RequestStart(IScript::Request& request, IScript::Delegate<Clock> clock) {
	METHOD_ENTRY(HeartVioliner::RequestStart);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(clock);
	clock->Play();
}

void HeartVioliner::RequestPause(IScript::Request& request, IScript::Delegate<Clock> clock) {
	METHOD_ENTRY(HeartVioliner::RequestPause);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(clock);
	clock->Pause();
}

void HeartVioliner::RequestNow(IScript::Request& request, IScript::Delegate<Clock> clock) {
	METHOD_ENTRY(HeartVioliner::RequestNow);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(clock);
	int64_t n = clock->Now();

	request.DoLock();
	request << n;
	request.UnLock();
}

void HeartVioliner::RequestPush(IScript::Request& request, IScript::Delegate<Queue> queue, int64_t timeStamp, IScript::Request::Ref obj) {
	METHOD_ENTRY(HeartVioliner::RequestPush);
	CHECK_REFERENCES(obj);
	CHECK_DELEGATE(queue);

	queue->Push(request, obj, timeStamp);
}

void HeartVioliner::RequestPop(IScript::Request& request, IScript::Delegate<Queue> queue, int64_t timeStamp) {
	METHOD_ENTRY(HeartVioliner::RequestPop);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(queue);

	queue->ExecuteWithTimeStamp(request, timeStamp);
}

void HeartVioliner::RequestClear(IScript::Request& request, IScript::Delegate<Queue> queue) {
	METHOD_ENTRY(HeartVioliner::RequestClear);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(queue);
	queue->Clear(request);
}

void HeartVioliner::RequestListen(IScript::Request& request, IScript::Delegate<Queue> queue, IScript::Request::Ref listener) {
	METHOD_ENTRY(HeartVioliner::RequestListen);
	CHECK_REFERENCES_WITH_TYPE(listener, IScript::Request::FUNCTION);
	CHECK_DELEGATE(queue);
	queue->Listen(request, listener);
}

void HeartVioliner::RequestCreateClock(IScript::Request& request, int64_t interval, int64_t start) {
	METHOD_ENTRY(HeartVioliner::RequestCreateClock);
	TShared<Clock> c = TShared<Clock>::Make(timerFactory, bridgeSunset, interval, start, true);

	request.DoLock();
	request << delegate(c());
	request.UnLock();
}

void HeartVioliner::RequestSetClock(IScript::Request& request, IScript::Delegate<Clock> clock, int64_t start) {
	METHOD_ENTRY(HeartVioliner::RequestSetClock);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(clock);
	clock->SetClock(start);
}

void HeartVioliner::RequestAttach(IScript::Request& request, IScript::Delegate<Clock> clock, IScript::Delegate<Queue> queue) {
	METHOD_ENTRY(HeartVioliner::RequestAttach);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(clock);
	CHECK_DELEGATE(queue);

	queue->Attach(clock.Get());
}

void HeartVioliner::RequestDetach(IScript::Request& request, IScript::Delegate<Queue> queue) {
	METHOD_ENTRY(HeartVioliner::RequestDetach);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(queue);

	queue->Detach();
}


/*
#include "../../Driver/Thread/Pthread/ZThreadPthread.h"
#include "../../Driver/Script/Lua/ZScriptLua.h"
#include "../../Driver/Timer/Freeglut/ZTimerFreeglut.h"

int HeartVioliner::main(int argc, char* argv[]) {
	ZThreadPthread thread;
	ZScriptLua script(thread);
	TFactory<ZTimerFreeglut, ITimer> fact;
	BridgeSunset bridgeSunset(thread, 3, 3);
	HeartVioliner hv(thread, fact, bridgeSunset);
	IScript::Request& request = script.GetDefaultRequest();
	IScript::Request::Ref ref = request.Load(String(
		"print('HeartVioliner::main()')\n"
		"local queue = HeartVioliner.CreateQueue()\n"
		"HeartVioliner.Listen(queue, function(v) print('Triggered! ' .. v) end)\n"
		"HeartVioliner.Push(queue, 0, 'hello')\n"
		"HeartVioliner.Push(queue, 1234, 'world')\n"
		"HeartVioliner.Push(queue, 3456, 1024)\n"
		"HeartVioliner.Pop(queue, 3333)\n"
		"print('STATUS' .. (queue == queue and 'YES' or 'NO'))\n"
		"print('IO' .. (io == nil and 'YES' or 'NO'))\n"
		));
	request << global << key("HeartVioliner") << hv << endtable;
	request << global << key("io") << nil << endtable;
	request << global << key("debug") << nil << endtable;
	request.Push();
	request.Call(Sync, ref);
	request.Pop();
	request.Dereference(ref);
	return 0;
}
*/