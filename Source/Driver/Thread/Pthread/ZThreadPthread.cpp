#include "ZThreadPthread.h"
#if USE_STATIC_THIRDPARTY_LIBRARIES
#define PTW32_STATIC_LIB
#endif

#if defined(_MSC_VER) && _MSC_VER > 1200
// #define PREFER_NATIVE_THREAD
#endif

#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

using namespace PaintsNow;

class LockImpl : public IThread::Lock {
public:
	size_t lockCount;
	#if defined(_WIN32) && defined(_DEBUG)
	DWORD threadID;
	#endif
	#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	CRITICAL_SECTION cs;
	#else
	pthread_mutex_t lock;
	#endif
};

class SemaphoreImpl : public IThread::Semaphore {
public:
	sem_t sem;
};

class ThreadImpl : public IThread::Thread {
public:
	pthread_t thread;
	TWrapper<bool, IThread::Thread*, size_t> proxy;
	size_t index;
	bool running;
	bool managed;
};

class EventImpl : public IThread::Event {
public:
#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	CONDITION_VARIABLE cond;
#else
	pthread_cond_t cond;
#endif
};

static void* _ThreadProc(void* param)
{
	ThreadImpl* thread = reinterpret_cast<ThreadImpl*>(param);
	thread->running = true;
	bool deleteSelf = thread->proxy(thread, thread->index);

	if (deleteSelf) {
		thread->running = false;
		delete thread;
	}

	pthread_detach(pthread_self());
	pthread_exit(nullptr);

	return nullptr;
}

#if defined(_WIN32) || defined(WIN32)
class Win32ThreadHelper {
public:
	Win32ThreadHelper() {
		pthread_win32_process_attach_np();
	}

	~Win32ThreadHelper() {
		pthread_win32_process_detach_np();
	}
};
#endif

ZThreadPthread::ZThreadPthread(bool init) : initOnWin32(init) {
#if defined(_WIN32) || defined(WIN32)
	if (initOnWin32) {
		static Win32ThreadHelper threadHelper;
	}
#endif
	minPriority = sched_get_priority_max(SCHED_RR);
	maxPriority = sched_get_priority_min(SCHED_RR);
}

ZThreadPthread::~ZThreadPthread() {
}

void ZThreadPthread::Wait(Thread* th) {
	ThreadImpl* t = static_cast<ThreadImpl*>(th);
	void* param;
	pthread_join(t->thread, &param);
}

IThread::Thread* ZThreadPthread::NewThread(const TWrapper<bool, IThread::Thread*, size_t>& wrapper, size_t index, bool realtime) {
	ThreadImpl* t = new ThreadImpl();
	t->proxy = wrapper;
	t->index = index;
	t->managed = true;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	sched_param sched;
	sched.sched_priority = realtime ? maxPriority : minPriority;
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_attr_setschedparam(&attr, &sched);
//	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&t->thread, nullptr, _ThreadProc, t);
	pthread_attr_destroy(&attr);

#if defined(_WIN32) || defined(WIN32)
	// pthread_detach(t->thread);
#endif

	return t;
}


bool ZThreadPthread::IsThreadRunning(Thread* th) const {
	assert(th != nullptr);
	ThreadImpl* thread = static_cast<ThreadImpl*>(th);
	return thread->running;
}

void ZThreadPthread::DeleteThread(Thread* thread) {
	assert(thread != nullptr);
	ThreadImpl* t = static_cast<ThreadImpl*>(thread);
	if (t->managed) {
	//	pthread_detach(t->thread);
	}
	delete t;
}

IThread::Thread* ZThreadPthread::OpenCurrentThread() const {
	ThreadImpl* t = new ThreadImpl();
	t->index = 0;
	t->thread = pthread_self();
	t->managed = false;

	return t;
}

bool ZThreadPthread::EqualThread(IThread::Thread* l, IThread::Thread* r) const {
	assert(l != nullptr);
	assert(r != nullptr);

	ThreadImpl* lhs = static_cast<ThreadImpl*>(l);
	ThreadImpl* rhs = static_cast<ThreadImpl*>(r);
	return pthread_equal(lhs->thread, rhs->thread) != 0;
}

IThread::Lock* ZThreadPthread::NewLock() {
	LockImpl* lock = new LockImpl();
	lock->lockCount = 0;

#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	::InitializeCriticalSection(&lock->cs);
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	// pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&lock->lock, &attr);
	pthread_mutexattr_destroy(&attr);
#endif

	return lock;
}

bool ZThreadPthread::DoLock(Lock* l) {
	assert(l != nullptr);
	LockImpl* lock = static_cast<LockImpl*>(l);
#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	bool ret = true;
	::EnterCriticalSection(&lock->cs);
#else
	bool ret = pthread_mutex_lock(&lock->lock) == 0;
#endif
	lock->lockCount++;

#if defined(_WIN32) && defined(_DEBUG)
	lock->threadID = ::GetCurrentThreadId();
#endif

	return ret;
}

bool ZThreadPthread::UnLock(Lock* l) {
	assert(l != nullptr);
	LockImpl* lock = static_cast<LockImpl*>(l);
	lock->lockCount--;

#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	::LeaveCriticalSection(&lock->cs);
	return true;
#else
	return pthread_mutex_unlock(&lock->lock) == 0;
#endif
}

bool ZThreadPthread::TryLock(Lock* l) {
	assert(l != nullptr);
	LockImpl* lock = static_cast<LockImpl*>(l);
#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	bool success = ::TryEnterCriticalSection(&lock->cs) != 0;
#else
	bool success = pthread_mutex_trylock(&lock->lock) == 0;
#endif
	if (success) {
		lock->lockCount++;
	}

	return success;
}

void ZThreadPthread::DeleteLock(Lock* l) {
	assert(l != nullptr);
	LockImpl* lock = static_cast<LockImpl*>(l);
#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	::DeleteCriticalSection(&lock->cs);
#else
	pthread_mutex_destroy(&lock->lock);
#endif
	delete lock;
}

size_t ZThreadPthread::GetLockCount(Lock* l) {
	assert(l != nullptr);
	LockImpl* lock = static_cast<LockImpl*>(l);
	assert(lock->lockCount != 0); // danger!!!
	return lock->lockCount;
}

IThread::Semaphore* ZThreadPthread::NewSemaphore(int initValue) {
	SemaphoreImpl* sem = new SemaphoreImpl();
	sem_init(&sem->sem, 0, initValue);
	return sem;
}

void ZThreadPthread::Signal(Semaphore* s) {
	assert(s != nullptr);
	SemaphoreImpl* semaphore = static_cast<SemaphoreImpl*>(s);
	sem_post(&semaphore->sem);
//	OutputDebugString("POSTED\n");
}

#if defined(_MSC_VER) && !defined(PREFER_NATIVE_THREAD)
#include <Windows.h>
#define CLOCK_MONOTONIC 0
int clock_gettime(int, struct timespec *spec)      //C-file part
{
	__int64 wintime; GetSystemTimeAsFileTime((FILETIME*)&wintime);
	wintime -= (__int64)116444736000000000;  //1jan1601 to 1jan1970
	spec->tv_sec = wintime / 10000000;           //seconds
	spec->tv_nsec = wintime % 10000000 * 100;      //nano-seconds
	return 0;
}
#endif


void ZThreadPthread::Wait(Semaphore* s) {
	assert(s != nullptr);
	SemaphoreImpl* semaphore = static_cast<SemaphoreImpl*>(s);
//	OutputDebugString("FORWAITED\n");
	sem_wait(&semaphore->sem);
//	OutputDebugString("WAITED\n");
}

void ZThreadPthread::DeleteSemaphore(Semaphore* s) {
	assert(s != nullptr);
	SemaphoreImpl* semaphore = static_cast<SemaphoreImpl*>(s);
	sem_destroy(&semaphore->sem);
	delete semaphore;
}

IThread::Event* ZThreadPthread::NewEvent() {
	EventImpl* ev = new EventImpl();
#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	::InitializeConditionVariable(&ev->cond);
#else
	pthread_cond_init(&ev->cond, nullptr);
#endif
	return ev;
}

void ZThreadPthread::Signal(Event* event, bool broadcast) {
	assert(event != nullptr);
	EventImpl* ev = static_cast<EventImpl*>(event);
	int ret = 0;

#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	if (broadcast) {
		::WakeAllConditionVariable(&ev->cond);
	} else {
		::WakeConditionVariable(&ev->cond);
	}
#else
	if (broadcast) {
		ret = pthread_cond_broadcast(&ev->cond);
	} else {
		ret = pthread_cond_signal(&ev->cond);
	}
#endif
}

void ZThreadPthread::Wait(Event* event, Lock* lock, size_t timeout) {
	assert(event != nullptr);
	assert(lock != nullptr);
	EventImpl* ev = static_cast<EventImpl*>(event);
	LockImpl* l = static_cast<LockImpl*>(lock);
	l->lockCount--; // it's safe because we still hold this lock
#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	::SleepConditionVariableCS(&ev->cond, &l->cs, (DWORD)timeout);
#else
	timespec ts;
	#ifdef _WIN32
	clock_gettime(CLOCK_MONOTONIC, &ts);

	ts.tv_sec = ts.tv_sec + (long)(timeout / 1000);
	ts.tv_nsec = ts.tv_nsec + (timeout % 1000) * 1000000;
	ts.tv_sec += ts.tv_nsec / 1000000000;
	ts.tv_nsec = ts.tv_nsec % 1000000000;
	#else
	struct timeval now;
	gettimeofday(&now, NULL);
	ts.tv_sec = now.tv_sec + (long)(timeout / 1000);
	ts.tv_nsec = (now.tv_usec + (timeout % 1000)) * 1000;
	ts.tv_sec += ts.tv_nsec / 1000000000;
	ts.tv_nsec = ts.tv_nsec % 1000000000;
	#endif

	pthread_cond_timedwait(&ev->cond, &l->lock, &ts);
#endif
	l->lockCount++; // it's also safe because we has already take lock before returning from pthread_cond_wait
}

void ZThreadPthread::Wait(Event* event, Lock* lock) {
	assert(event != nullptr);
	assert(lock != nullptr);
	EventImpl* ev = static_cast<EventImpl*>(event);
	LockImpl* l = static_cast<LockImpl*>(lock);
	l->lockCount--; // it's safe because we still hold this lock
#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	::SleepConditionVariableCS(&ev->cond, &l->cs, INFINITE);
#else
	pthread_cond_wait(&ev->cond, &l->lock);
#endif
	l->lockCount++; // it's also safe because we has already take lock before returning from pthread_cond_wait
}

void ZThreadPthread::DeleteEvent(Event* event) {
	assert(event != nullptr);
	EventImpl* ev = static_cast<EventImpl*>(event);
#if defined(PREFER_NATIVE_THREAD) && defined(_WIN32) && defined(_MSC_VER) && _MSC_VER > 1200
	// Win32 have no DeleteConditionVariable...
#else
	pthread_cond_destroy(&ev->cond);
#endif

	delete ev;
}
