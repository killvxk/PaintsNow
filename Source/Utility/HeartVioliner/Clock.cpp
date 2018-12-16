#include "Clock.h"
#include <ctime>
#include <iterator>
using namespace PaintsNow;
using namespace PaintsNow::NsHeartVioliner;
using namespace PaintsNow::NsBridgeSunset;

Clock::Clock(ITimer& base, BridgeSunset& b, int64_t interval, int64_t start, bool mergeTicks) : bridgeSunset(b), timerBase(base), now(start), offset(0) {
	if (mergeTicks) {
		Flag() |= CLOCK_MERGE_TICKS;
	}

	eventTickerLock = bridgeSunset.GetThreadApi().NewLock();

	Play();
	timer = base.StartTimer((size_t)interval, Wrap(this, &Clock::OnTimer));
}

void Clock::AddTicker(ITask* task, void* context) {
	bridgeSunset.GetThreadApi().DoLock(eventTickerLock);
	tickerTasks[task] = context;
	bridgeSunset.GetThreadApi().UnLock(eventTickerLock);
}

void Clock::RemoveTicker(ITask* task) {
	bridgeSunset.GetThreadApi().DoLock(eventTickerLock);
	tickerTasks.erase(task);
	bridgeSunset.GetThreadApi().UnLock(eventTickerLock);
}

Clock::~Clock() {
	assert(timer == nullptr);
	bridgeSunset.GetThreadApi().DeleteLock(eventTickerLock);
}

TObject<IReflect>& Clock::operator () (IReflect& reflect) {
	ReflectClass(Clock);

	if (reflect.IsReflectMethod()) {
		ReflectMethod(Now);
		ReflectMethod(Play);
		ReflectMethod(Pause);
		ReflectMethod(SetClock);
		ReflectMethod(Stop);
		ReflectMethod(IsRunning);
	}

	return *this;
}

void Clock::ScriptUninitialize(IScript::Request& request) {
	Stop();
	SharedTiny::ScriptUninitialize(request);
}

inline int64_t Clock::GetFullClock() const {
	return ITimer::GetSystemClock();
}

void Clock::Play() {
	Resume();
}

void Clock::Resume() {
	offset = GetFullClock() - now;
	Flag() |= TINY_ACTIVATED;
}

void Clock::Pause() {
	Flag() &= ~TINY_ACTIVATED;
}

bool Clock::IsRunning() const {
	return !!(Flag() & TINY_ACTIVATED);
}

void Clock::SetClock(int64_t w) {
	now = w;
	offset = GetFullClock() - w;
}

int64_t Clock::Now() const {
	return now;
}

void Clock::Stop() {
	Pause();

	ITimer::Timer* t = timer;
	timer = nullptr;
	timerBase.StopTimer(t);
}

void Clock::Execute(void* context) {
	int64_t last = now;
	now = GetFullClock() - offset;

	bridgeSunset.GetThreadApi().DoLock(eventTickerLock);
	std::vector<std::pair<ITask*, void*> > tasks;
	tasks.reserve(tickerTasks.size());
	std::copy(tickerTasks.begin(), tickerTasks.end(), std::back_inserter(tasks));
	bridgeSunset.GetThreadApi().UnLock(eventTickerLock);

	for (size_t i = 0; i < tasks.size(); i++) {
		std::pair<ITask*, void*>& task = tasks[i];
		task.first->Execute(task.second != nullptr ? task.second : context);
	}

	Flag() &= ~TINY_MODIFIED;
}

void Clock::OnTimer(size_t interval) {
	if (timer != nullptr) {
		if (IsRunning()) {
			const FLAG flagMergeTick = CLOCK_MERGE_TICKS | TINY_MODIFIED;
			if ((Flag() & flagMergeTick) == flagMergeTick) return;

			Flag() |= TINY_MODIFIED;
			bridgeSunset.Dispatch(this);
		} else {
			offset = GetFullClock() - now;
		}
	}
}
