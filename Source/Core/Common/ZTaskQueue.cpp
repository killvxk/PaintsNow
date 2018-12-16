#include "ZTaskQueue.h"
#include "ZThreadPool.h"
using namespace PaintsNow;

ZTaskQueue::RingBuffer::RingBuffer(uint32_t maxTaskCount) : pushIndex(0), popIndex(0), taskBuffer(maxTaskCount) {}

ZTaskQueue::ZTaskQueue(uint32_t idCount, uint32_t maxTaskCount) : committed(false) {
	assert((maxTaskCount & (maxTaskCount - 1)) == 0);
	ringBuffers.resize(idCount, RingBuffer(maxTaskCount));
}

ZTaskQueue::~ZTaskQueue() {
	// abort all
	Abort(nullptr);
}

void ZTaskQueue::Flush(ZThreadPool& threadPool) {
	if (!committed) {
		committed = true;
		threadPool.Push(this);
	}
}

void ZTaskQueue::OnOperation(void (ITask::*operation)(void*), void* context) {
	for (size_t i = 0; i < ringBuffers.size(); i++) {
		RingBuffer& ringBuffer = ringBuffers[i];
		uint32_t indexMask = (uint32_t)ringBuffer.taskBuffer.size() - 1;
		while (ringBuffer.popIndex != ringBuffer.pushIndex) {
			InvokeOperation(ringBuffer.taskBuffer[ringBuffer.popIndex], operation, context);
			ringBuffer.popIndex = (ringBuffer.popIndex + 1) & indexMask;
		}
	}

	committed = false;
}

void ZTaskQueue::InvokeOperation(std::pair<ITask*, void*>& task, void (ITask::*operation)(void*), void* context) {
	(task.first->*operation)(context);
}

void ZTaskQueue::Execute(void* context) {
	OnOperation(&ITask::Execute, context);
}

void ZTaskQueue::Suspend(void* context) {}

void ZTaskQueue::Resume(void* context) {}

void ZTaskQueue::Abort(void* context) {
	OnOperation(&ITask::Abort, context);
}

bool ZTaskQueue::Continue() const {
	return true;
}

void ZTaskQueue::Push(uint32_t id, ITask* task, void* tag) {
	assert(id < (uint32_t)ringBuffers.size());
	RingBuffer& ringBuffer = ringBuffers[id];
	uint32_t indexMask = (uint32_t)ringBuffer.taskBuffer.size() - 1;
	if (ringBuffer.pushIndex == ringBuffer.popIndex) {
		ringBuffer.backBuffer.push_back(std::make_pair(task, tag));
	} else {
		ringBuffer.taskBuffer[ringBuffer.pushIndex] = std::make_pair(task, tag);
		ringBuffer.pushIndex = (ringBuffer.pushIndex + 1) & indexMask;
	}
}