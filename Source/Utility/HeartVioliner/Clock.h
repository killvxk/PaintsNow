// Clock.h
// By PaintDream (paintdream@paintdream.com)
// 2015-1-5
//

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "../../Core/Common/ZType.h"
#include "../../Core/Interface/IScript.h" 
#include "../../Core/Object/Tiny.h"
#include "../../Core/Interface/ITimer.h" 
#include "../../Core/Template/TFactory.h"
#include "../../Core/Template/TEvent.h"
#include "../BridgeSunset/BridgeSunset.h"
#include <map>
#include <string>

namespace PaintsNow {
	namespace NsHeartVioliner {
		class Clock : public SharedTiny, public QuickTask {
		public:
			Clock(ITimer& base, NsBridgeSunset::BridgeSunset& bridgeSunset, int64_t interval, int64_t start, bool mergeTicks);
			virtual ~Clock();
			enum { CLOCK_MERGE_TICKS = TINY_CUSTOM_BEGIN };
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			int64_t Now() const;
			void Play();
			void Pause();
			void Resume();
			void SetClock(int64_t tm);
			void Stop();
			bool IsRunning() const;
			void AddTicker(ITask* task, void* context);
			void RemoveTicker(ITask* task);

		private:
			int64_t GetFullClock() const;
			virtual void ScriptUninitialize(IScript::Request& request);
			void OnTimer(size_t interval);
			virtual void Execute(void* context) override;

		private:
			NsBridgeSunset::BridgeSunset& bridgeSunset;
			IThread::Lock* eventTickerLock;
			std::map<ITask*, void*> tickerTasks;
			ITimer& timerBase;
			ITimer::Timer* timer;
			int64_t now;
			int64_t offset;
		};
	}
}

#endif // __CLOCK_H__