// WorkDispatcher.h
// By PaintDream (paintdream@paintdream.com)
// 2018-2-1
//

#ifndef __WORKDISPATCHER_H__
#define __WORKDISPATCHER_H__

#include "../../Core/Interface/IScript.h"
#include "../../Core/Interface/INetwork.h"
#include "Looper.h"

namespace PaintsNow {
	namespace NsEchoLegend {
		class WorkDispatcher : public Looper {
		public:
			WorkDispatcher(NsBridgeSunset::BridgeSunset& bridgeSunset, INetwork& network, ITunnel::Dispatcher* disp);
			virtual bool Activate();
			virtual void Deactivate();
			ITunnel::Dispatcher* GetDispatcher() const;
			virtual TObject<IReflect>& operator () (IReflect& reflect);

		protected:
			ITunnel::Dispatcher* dispatcher;
		};
	}
}

#endif // __WORKDISPATCHER_H__