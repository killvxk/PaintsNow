// ZThreadPool.h
// By PaintDream (paintdream@paintdream.com)
// 2014-12-14
//

#ifndef __ZTHREADPOOL_H__
#define __ZTHREADPOOL_H__

#include "../Interface/IThread.h"
#include "../Interface/ITask.h"
#include "../Template/TProxy.h"
#include "../Template/TAtomic.h"

#include <queue>
#include <vector>

namespace PaintsNow {
	class ZThreadPool {
	public:
		ZThreadPool(IThread& threadApi, size_t threadCount, bool realtimeThread);

	public:
		virtual ~ZThreadPool();
		bool Push(ITask* task);
		size_t GetThreadCount() const;
		size_t GetCurrentThreadIndex() const;
		void SetThreadContext(size_t index, void* context);
		void* GetThreadContext(size_t index) const;
		IThread& GetThreadApi();
		void Reset(bool reload);
		void Init();
		enum { BLOCK_TASK_SIZE = 2048 };

	protected:
		struct TaskBlock {
			TaskBlock(uint32_t offset);
			TAtomic<TaskBlock*> next;
			TAtomic<uint32_t> finishCount;
			TAtomic<uint32_t> isHead;
			uint32_t offset;
			ITask* tasks[BLOCK_TASK_SIZE];
		};
		
		bool Run(IThread::Thread* thread, size_t threadID);
		std::pair<ITask* volatile*, TaskBlock*> Locate(TAtomic<TaskBlock*>& taskBlockHead, uint32_t id);
		std::pair<ITask* volatile*, TaskBlock*> Pop();

	protected:
		IThread& threadApi;
		TAtomic<uint32_t> pushCounter;
		TAtomic<uint32_t> popCounter;
		TAtomic<TaskBlock*> taskBlockPushHead;
		TAtomic<TaskBlock*> taskBlockPopHead;
		TAtomic<uint32_t> liveThreadCount;
		uint32_t waitEventCounter;
		IThread::Lock* eventLock;
		IThread::Event* eventPump;
		size_t threadCount;

		std::vector<std::pair<IThread::Thread*, void*> > threadHandles;
		volatile bool finish;
		bool realtime;
	};
}


#endif // __ZTHREADPOOL_H__
