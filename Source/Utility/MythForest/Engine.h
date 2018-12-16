// Engine.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-1
//

#ifndef __ENGINE_H__
#define __ENGINE_H__

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <set>
#include "../../Core/Object/Tiny.h"
#include "../../Core/Template/TMap.h"
#include "../../Core/Interface/Interfaces.h"
#include "../../Core/Common/ZTaskQueue.h"

namespace PaintsNow {
	class Interfaces;
	namespace NsBridgeSunset {
		class BridgeSunset;
	}
	namespace NsFluteLeaves {
		class FluteLeaves;
	}
	namespace NsSnowyStream {
		class SnowyStream;
	}
	namespace NsMythForest {
		class Unit;
		class Entity;
		class MythForest;
		class Module;
		class Engine {
		public:
			Engine(Interfaces& interfaces, NsBridgeSunset::BridgeSunset& bridgeSunset, NsFluteLeaves::FluteLeaves& fluteLeaves, NsSnowyStream::SnowyStream& snowyStream, NsMythForest::MythForest& mythForest, size_t warpCount);

			virtual ~Engine();
			void Clear();
			void InstallModule(Module* module);
			void UninstallModule(Module* module);
			Module* GetComponentModuleFromName(const String& name) const;
			const unordered_map<String, Module*>& GetModuleMap() const;

			void TickFrame();
			uint32_t GetCurrentWarpIndex() const;
			uint32_t GetWarpCount() const;
			void QueueExternalRoutine(ITask* task);
			void QueueRoutine(Unit* unit, ITask* task);
			void CommitRoutines();

			Interfaces& interfaces;
			NsBridgeSunset::BridgeSunset& bridgeSunset;
			NsSnowyStream::SnowyStream& snowyStream;
			NsFluteLeaves::FluteLeaves& fluteLeaves;
			NsMythForest::MythForest& mythForest;

			class RepostableTaskQueue : public ZTaskQueue {
			public:
				RepostableTaskQueue(Engine* engine, uint32_t idCount, uint32_t maxTaskCount);
				virtual ~RepostableTaskQueue();
				virtual void InvokeOperation(std::pair<ITask*, void*>& task, void (ITask::*operation)(void*), void* context) override;

			protected:
				Engine* engine;
			};

			friend class RepostableTaskQueue;

		protected:
			unordered_map<String, Module*> modules;
			std::vector<RepostableTaskQueue> taskQueueGrid;
		};
	}
}

#endif // __ENGINE_H__