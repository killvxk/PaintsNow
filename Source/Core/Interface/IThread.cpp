#include "IThread.h"

using namespace PaintsNow;

IThread::~IThread() {}

LockGuard::LockGuard(IThread& t, IThread::Lock* l) : thread(t), lock(l) {
	thread.DoLock(lock);
}

LockGuard::~LockGuard() {
	thread.UnLock(lock);
}