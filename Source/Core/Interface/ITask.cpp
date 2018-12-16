#include "ITask.h"

using namespace PaintsNow;

ITask::~ITask() {

}

void QuickTask::Suspend(void* context) {}
void QuickTask::Resume(void* context) {}
void QuickTask::Abort(void* context) {
	delete this;
}
bool QuickTask::Continue() const { return true; }
