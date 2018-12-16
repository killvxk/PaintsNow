// ZTaskQueue.h
// PaintDream (paintdream@paintdream.com)
// 2018-10-30
//

#ifndef __ZTASKQUEUE_H__
#define __ZTASKQUEUE_H__

#include "../Template/TAtomic.h"
#include "../Interface/ITask.h"
#include "ZType.h"
#include <vector>

namespace PaintsNow {
	// ZTaskQueue is single-thread read + single-thread/write task ring buffer
	// All tasks in the same task queue are scheduled in sequence. So no lock is needed for themselves ...
	// ZTaskQueue is also an ITask.
	class ZThreadPool;
	class ZTaskQueue : public ITask {
	public:
		ZTaskQueue(uint32_t idCount, uint32_t maxTaskCount);
		virtual ~ZTaskQueue();
		void Push(uint32_t id, ITask* task, void* tag);
		void Flush(ZThreadPool& threadPool);

	protected:
		virtual void InvokeOperation(std::pair<ITask*, void*>& task, void (ITask::*operation)(void*), void* context);

		virtual void Execute(void* context) override final;
		virtual void Suspend(void* context) override final;
		virtual void Resume(void* context) override final;
		virtual void Abort(void* context) override final;
		virtual bool Continue() const override final;

	protected:
		void OnOperation(void (ITask::*operation)(void*), void* context);
		// Wait-free Ring Buffer
		struct RingBuffer {
			RingBuffer(uint32_t size);
			uint32_t pushIndex;
			uint32_t popIndex;
			std::vector<std::pair<ITask*, void*> > taskBuffer;
			std::vector<std::pair<ITask*, void*> > backBuffer;
		};

		std::vector<RingBuffer> ringBuffers;
		bool committed;
	};
}

#endif // __ZTASKQUEUE_H__