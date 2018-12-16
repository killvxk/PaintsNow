// ScreenSpaceTrace.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __SCREENSPACETRACE_H__
#define __SCREENSPACETRACE_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class ScreenSpaceTrace : public IShader {
		public:
			ScreenSpaceTrace();
			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};
	}
}


#endif // __SCREENSPACETRACE_H__
