#define MAX_YIELD_COUNT 64
#ifdef _WIN32
#define MAX_WAIT_MILLISECONDS 50
#else
// reduce to 200ms due to low performance on gettimeofday
#define MAX_WAIT_MILLISECONDS 200
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1200
#define _WIN32_WINNT 0x0500
#include <windows.h>
inline void YieldThread() {
	if (!::SwitchToThread()) {
		for (int i = 0; i < 16; i++)
		{
			YieldProcessor();
		}
	}
}

#else
#include <thread>
inline void YieldThread() {
	std::this_thread::yield();
}

#endif

#include "ZThreadPool.h"
#include "../Template/TProxy.h"
#include <cassert>
#include <ctime>
#include <cstdlib>

using namespace PaintsNow;

ZThreadPool::ZThreadPool(IThread& t, size_t tc, bool r) : threadApi(t), threadCount(tc), finish(false), realtime(r), taskBlockPopHead(nullptr), taskBlockPushHead(nullptr) {
	eventLock = threadApi.NewLock();
	eventPump = threadApi.NewEvent();
	Init();
}

void ZThreadPool::Init() {
	finish = false;
	TaskBlock* initBlock = new TaskBlock(0);
	initBlock->isHead = 1;
	taskBlockPopHead.store(initBlock, std::memory_order_relaxed);
	taskBlockPushHead.store(initBlock, std::memory_order_relaxed);
	pushCounter.store(0, std::memory_order_relaxed);
	popCounter.store(0, std::memory_order_relaxed);
	liveThreadCount.store(0, std::memory_order_relaxed);
	waitEventCounter = 0;

	assert(threadHandles.empty());
	threadHandles.resize(threadCount);

	for (size_t i = 0; i < threadHandles.size(); i++) {
		threadHandles[i] = std::make_pair(threadApi.NewThread(Wrap(this, &ZThreadPool::Run), i, realtime), (void*)nullptr);
	}
}

size_t ZThreadPool::GetThreadCount() const {
	return threadHandles.size();
}

thread_local size_t localThreadIndex = (size_t)-1;

size_t ZThreadPool::GetCurrentThreadIndex() const {
	return localThreadIndex;
}

IThread& ZThreadPool::GetThreadApi() {
	return threadApi;
}

void ZThreadPool::SetThreadContext(size_t id, void* context) {
	assert(id < threadHandles.size());
	threadHandles[id].second = context;
}

void* ZThreadPool::GetThreadContext(size_t id) const {
	assert(id < threadHandles.size());
	return threadHandles[id].second;
}

void ZThreadPool::Reset(bool reload) {
	// force kill all pending tasks
	size_t threadCount = threadHandles.size();
	finish = true;
	if (threadCount != 0) {
		uint32_t yieldCount = 0;
		while (liveThreadCount.load(std::memory_order_acquire
		) != 0) {
			if (yieldCount++ < MAX_YIELD_COUNT) {
				YieldThread();
			} else {
				threadApi.DoLock(eventLock);
				++waitEventCounter;
				threadApi.Wait(eventPump, eventLock, MAX_WAIT_MILLISECONDS);
				--waitEventCounter;
				threadApi.UnLock(eventLock);
				yieldCount = 0;
			}
		}

		// free blocks
		TaskBlock* block = (TaskBlock*)taskBlockPopHead.load(std::memory_order_acquire);
		assert((TaskBlock*)taskBlockPushHead.load(std::memory_order_acquire) == block);
		if (block != nullptr) {
			taskBlockPopHead = nullptr;
			taskBlockPushHead = nullptr;

			// must be the last one
			assert(block->next.load(std::memory_order_acquire) == nullptr);
			delete block;
		}

		for (size_t k = 0; k < threadCount; k++) {
			threadApi.DeleteThread(threadHandles[k].first);
		}

		threadHandles.clear();
	}

	if (reload) {
		Init();
	}
}


ZThreadPool::~ZThreadPool() {
	Reset(false);

	threadApi.DeleteEvent(eventPump);
	threadApi.DeleteLock(eventLock);
}

ZThreadPool::TaskBlock::TaskBlock(uint32_t off) : next(nullptr), offset(off), isHead(0), finishCount(0) {
	memset(tasks, 0, sizeof(tasks));
}

std::pair<ITask* volatile*, ZThreadPool::TaskBlock*> ZThreadPool::Locate(TAtomic<TaskBlock*>& p, uint32_t id) {
	id = id - 1;
	uint32_t offset = id & ~(BLOCK_TASK_SIZE - 1);
	TaskBlock* t;
	assert(p.load(std::memory_order_acquire) != nullptr);
	while ((t = (TaskBlock*)p.load(std::memory_order_acquire)) != nullptr) {
		if (t->offset == offset) {
			return std::make_pair(&t->tasks[id & (BLOCK_TASK_SIZE - 1)], t);
		}

		TaskBlock* next;
		do {
			next = (TaskBlock*)t->next.load(std::memory_order_acquire);
			if (next == nullptr) {
				TaskBlock* newBlock = new TaskBlock(t->offset + BLOCK_TASK_SIZE);
				TaskBlock* empty = nullptr;

				if (!t->next.compare_exchange_strong(empty, newBlock, std::memory_order_release)) {
					delete newBlock; // failed, free memory
				}
			}
		} while (next == nullptr);

		p.compare_exchange_strong(t, next, std::memory_order_release);
	}

	// never reach here
	assert(false);
	return std::make_pair((ITask* volatile*)nullptr, (ZThreadPool::TaskBlock*)nullptr);
}

bool ZThreadPool::Push(ITask* task) {
	assert(task != nullptr);
	assert(taskBlockPushHead.load(std::memory_order_acquire) != nullptr);
	if (!finish) {
		*Locate(taskBlockPushHead, ++pushCounter).first = task;// fetch_and_add
		uint32_t counter = waitEventCounter;
		if (counter != 0) {
			threadApi.Signal(eventPump, counter * 2 > threadCount);
		}

		return true;
	} else {
		task->Abort(nullptr);
		return false;
	}
}

std::pair<ITask* volatile*, ZThreadPool::TaskBlock*> ZThreadPool::Pop() {
	assert(taskBlockPopHead.load(std::memory_order_acquire) != nullptr);
	return Locate(taskBlockPopHead, ++popCounter);
}

#include <iostream>

bool ZThreadPool::Run(IThread::Thread* thread, size_t index) {
	// set thread local
	localThreadIndex = index;

	// fetch one and execute
	++liveThreadCount;

	while (true) {
		std::pair<ITask* volatile*, TaskBlock*> task = Pop();
		uint32_t yieldCount = 0;
		while (*task.first == nullptr) {
			if (finish) {
				break;
			}

			if (yieldCount++ < MAX_YIELD_COUNT) {
				YieldThread();
			} else {
				threadApi.DoLock(eventLock);
				++waitEventCounter;
				threadApi.Wait(eventPump, eventLock, MAX_WAIT_MILLISECONDS);
				--waitEventCounter;
				threadApi.UnLock(eventLock);
				yieldCount = 0;
			}
		}

		TaskBlock* block = task.second;
		ITask* p = *task.first;

		++block->finishCount;

		// free slot
		while (block->finishCount.load(std::memory_order_acquire) == BLOCK_TASK_SIZE) {
			uint32_t one = 1;
			if (block->isHead.compare_exchange_strong(one, 0u, std::memory_order_release)) {
				TaskBlock* t = block;
				TaskBlock* next;

				do {
					next = (TaskBlock*)t->next.load(std::memory_order_acquire);
					if (next == nullptr) {
						TaskBlock* newBlock = new TaskBlock(t->offset + BLOCK_TASK_SIZE);
						TaskBlock* empty = nullptr;

						if (!t->next.compare_exchange_strong(empty, newBlock, std::memory_order_release)) {
							delete newBlock; // failed, free memory
						}
					}
				} while (next == nullptr);

				block = next;
				block->isHead = 1;

				TaskBlock* cmp = t;
				taskBlockPushHead.compare_exchange_strong(cmp, block, std::memory_order_release);
				taskBlockPopHead.compare_exchange_strong(cmp, block, std::memory_order_release);

				delete t; // safe, any upstream node will not visit me
			} else {
				break;
			}
		}

		if (p != nullptr) {
			void* context = threadHandles[index].second;
			if (finish) {
				p->Abort(context);
			} else {
				p->Execute(context);
			}
		} else if (finish) {
			break;
		}
	}
	
	--liveThreadCount;
	return false; // manages IThread::Thread* by ourself
}

// Unit test

// #include <windows.h>
#include <cstdio>

struct MyTask {
public:
	MyTask() {}
	MyTask(size_t i, size_t d) : index(i), delay(d) {}
	void Run(size_t id) {
		// printf("[%d] Task %d delayed for %d ms\n", id, index, delay);
		// ::Sleep(delay);
	}

	void Exit(bool state) {
		// printf("Task %d terminated %s\n", index, state ? "successfully" : "unexpectly");
	}

	size_t index, delay;
};

/*
#include "../../../Driver/Thread/Pthread/ZThreadPthread.h"


int ZThreadPool::main(int argc, char* argv[]) {
	ZThreadPthread thread;
	ZThreadPool pool(thread, 3, false);
	const size_t TASK_COUNT = 100;
	MyTask tasks[TASK_COUNT];
	for (size_t i = 0; i < TASK_COUNT; i++) {
		tasks[i] = MyTask(i, rand() % 1000);
		pool.Push(std::make_pair(Wrap(&tasks[i], &MyTask::Run), Wrap(&tasks[i], &MyTask::Exit)));
	}

	getchar();
	return 0;
}
*/