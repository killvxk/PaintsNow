#include "Engine.h"
#include "Module.h"
#include "Entity.h"
#include "../../Core/Interface/IRender.h"
#include "../BridgeSunset/BridgeSunset.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsBridgeSunset;

Engine::Engine(Interfaces& pinterfaces, NsBridgeSunset::BridgeSunset& pbridgeSunset, NsFluteLeaves::FluteLeaves& pfluteLeaves, NsSnowyStream::SnowyStream& psnowyStream, NsMythForest::MythForest& pmythForest, size_t warpCount) : interfaces(pinterfaces), bridgeSunset(pbridgeSunset), fluteLeaves(pfluteLeaves), snowyStream(psnowyStream), mythForest(pmythForest) {
	size_t threadCount = bridgeSunset.GetThreadPool().GetThreadCount();
	if (warpCount == 0) {
		warpCount = threadCount * 2;
	}

	taskQueueGrid.resize(warpCount, RepostableTaskQueue(this, (uint32_t)threadCount, 1024));
}

Engine::~Engine() {
	assert(modules.empty());
}

void Engine::Clear() {
	for (unordered_map<String, Module*>::iterator it = modules.begin(); it != modules.end(); ++it) {
		(*it).second->Uninitialize();
	}

	modules.clear();
}

void Engine::InstallModule(Module* module) {
	modules[module->GetTinyUnique()->typeName] = module;
	module->Initialize();
}

void Engine::UninstallModule(Module* module) {
	unordered_map<String, Module*>::iterator it = modules.find(module->GetTinyUnique()->typeName);
	if (it != modules.end()) {
		(*it).second->Uninitialize();
	}
}

uint32_t Engine::GetWarpCount() const {
	return (uint32_t)taskQueueGrid.size();
}

Module* Engine::GetComponentModuleFromName(const String& name) const {
	unordered_map<String, Module*>::const_iterator it = modules.find(name);
	if (it != modules.end()) {
		return (*it).second;
	} else {
		return nullptr;
	}
}

const unordered_map<String, Module*>& Engine::GetModuleMap() const {
	return modules;
}

void Engine::TickFrame() {
	for (unordered_map<String, Module*>::iterator it = modules.begin(); it != modules.end(); ++it) {
		(*it).second->TickFrame();
	}
}

void Engine::QueueExternalRoutine(ITask* task) {
	bridgeSunset.Dispatch(task);
}

thread_local uint32_t WarpIndex = 0;

uint32_t Engine::GetCurrentWarpIndex() const {
	return WarpIndex;
}

void Engine::QueueRoutine(Unit* unit, ITask* task) {
	assert(unit != nullptr);
	uint32_t fromThreadIndex = bridgeSunset.GetThreadPool().GetCurrentThreadIndex();
	uint32_t toWarpIndex = unit->GetWarpIndex();
	
	if (WarpIndex != toWarpIndex) {
		unit->ReferenceObject(); // hold reference in case of invalid memory access.
		taskQueueGrid[toWarpIndex].Push(fromThreadIndex, task, unit);
	} else {
		task->Execute(bridgeSunset.GetThreadPool().GetThreadContext(fromThreadIndex));
	}
}

void Engine::CommitRoutines() {
	assert(WarpIndex < taskQueueGrid.size());

	taskQueueGrid[WarpIndex].Flush(bridgeSunset.GetThreadPool());
}

// RepostableTaskQueue
Engine::RepostableTaskQueue::RepostableTaskQueue(Engine* e, uint32_t idCount, uint32_t maxTaskCount) : engine(e), ZTaskQueue(idCount, maxTaskCount) {}

Engine::RepostableTaskQueue::~RepostableTaskQueue() {
	// Must cleanup remaining in derived class
	Abort(nullptr);
}

void Engine::RepostableTaskQueue::InvokeOperation(std::pair<ITask*, void*>& task, void (ITask::*operation)(void*), void* context) {
	Unit* unit = reinterpret_cast<Unit*>(task.second);
	uint32_t thisWarpIndex = (uint32_t)(this - &engine->taskQueueGrid[0]);
	if (thisWarpIndex = unit->GetWarpIndex()) {
		// Update WarpIndex
		WarpIndex = thisWarpIndex;
		(task.first->*operation)(context);
	} else {
		engine->QueueRoutine(unit, task.first);
	}

	unit->ReleaseObject();
}