// ZDebuggerWin.h
// By PaintDream (paintdream@paintdream.com)
//

#ifndef __ZDEBUGGERWIN_H__
#define __ZDEBUGGERWIN_H__

#include "../../../Core/Interface/IDebugger.h"

namespace PaintsNow {
	class ZDebuggerWin final : public IDebugger {
	public:
		ZDebuggerWin();
		virtual ~ZDebuggerWin();
		virtual void SetDumpHandler(const String& path, const TWrapper<bool>& handler);

		TWrapper<bool> handler;
		String path;
	};
}


#endif // __ZDEBUGGERWIN_H__
