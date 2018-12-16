// IDebugger.h
// By PaintDream (paintdream@paintdream.com)
// 2015-6-16
//

#ifndef __IDEBUGGER_H__
#define __IDEBUGGER_H__

#include "../Common/ZType.h"
#include "../Template/TProxy.h"
#include "IDevice.h"
#include <string>

namespace PaintsNow {
	class IDebugger : public IDevice {
	public:
		virtual ~IDebugger();
		virtual void SetDumpHandler(const String& path, const TWrapper<bool>& handler) = 0;
	};
}


#endif // __IDEBUGEER_H__