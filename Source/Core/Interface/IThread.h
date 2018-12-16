// IThread.h
// By PaintDream (paintdream@paintdream.com)
// 2014-12-14
//

#ifndef __ITHREAD_H__
#define __ITHREAD_H__


#include "../PaintsNow.h"
#include "../Common/ZType.h"
#include "../Template/TProxy.h"
#include "IDevice.h"

namespace PaintsNow {
	class IThread : public IDevice {
	public:
		virtual ~IThread();
		class Lock {};
		class Semaphore {};
		class Thread {};
		class Event {};
		

		virtual Thread* NewThread(const TWrapper<bool, Thread*, size_t>& wrapper, size_t index, bool realtime) = 0;
		virtual bool IsThreadRunning(Thread* thread) const = 0;
		virtual void Wait(Thread* thread) = 0;
		virtual void DeleteThread(Thread* thread) = 0;
		virtual IThread::Thread* OpenCurrentThread() const = 0;
		virtual bool EqualThread(Thread* lhs, Thread* rhs) const = 0;

		virtual Lock* NewLock() = 0;
		virtual bool DoLock(Lock* lock) = 0;
		virtual bool UnLock(Lock* lock) = 0;
		virtual bool TryLock(Lock* lock) = 0;
		virtual size_t GetLockCount(Lock* lock) = 0;
		virtual void DeleteLock(Lock* lock) = 0;

		virtual Semaphore* NewSemaphore(int initValue) = 0;
		virtual void Signal(Semaphore* semaphore) = 0;
		virtual void Wait(Semaphore* semaphore) = 0;
		virtual void DeleteSemaphore(Semaphore* semphore) = 0;

		virtual Event* NewEvent() = 0;
		virtual void Signal(Event* event, bool broadcast) = 0;
		virtual void Wait(Event* event, Lock* lock) = 0;
		virtual void Wait(Event* event, Lock* lock, size_t timeout) = 0;
		virtual void DeleteEvent(Event* event) = 0;
	};

	class LockGuard {
	public:
		LockGuard(IThread& thread, IThread::Lock* lock);
		~LockGuard();
	private:
		IThread& thread;
		IThread::Lock* lock;
	};
}


#endif // __ITHREAD_H__