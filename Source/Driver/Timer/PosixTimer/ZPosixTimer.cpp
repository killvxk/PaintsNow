#if !defined(_WIN32) && !defined(WIN32)
#include "ZPosixTimer.h"
#include <cassert>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

using namespace PaintsNow;

class PosixTimerImpl : public ITimer::Timer {
public:
	TWrapper<void, size_t> wrapper;
	size_t interval;
	timer_t timerID;
};

static void TimerFunc(sigval s) {
	PosixTimerImpl* q = reinterpret_cast<PosixTimerImpl*>(s.sival_ptr);
	q->wrapper(q->interval);
}

ZPosixTimer::ZPosixTimer() {
}

ZPosixTimer::~ZPosixTimer() {
}

void ZPosixTimer::StopTimer(Timer* timer) {
	PosixTimerImpl* impl = static_cast<PosixTimerImpl*>(timer);
	timer_delete(impl->timerID);
	delete impl;
}

size_t ZPosixTimer::GetTimerInterval(Timer* timer) const {
	PosixTimerImpl* impl = static_cast<PosixTimerImpl*>(timer);
	return impl->interval;
}

ITimer::Timer* ZPosixTimer::StartTimer(size_t inter, const TWrapper<void, size_t>& w) {
	PosixTimerImpl* impl = new PosixTimerImpl();
	impl->wrapper = w;
	impl->interval = inter;

	sigevent sev;
	itimerspec its;
	// long long freq_nanosecs;
	// sigset_t mask;

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = TimerFunc;
	sev.sigev_value.sival_ptr = this;
	timer_create(CLOCK_REALTIME, &sev, &impl->timerID);

	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 0;

	timer_settime(impl->timerID, 0, &its, nullptr);
	return impl;
}

#endif // !_WIN32
