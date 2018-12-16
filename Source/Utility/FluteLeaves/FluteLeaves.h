// FluteLeaves.h
// By PaintDream (paintdream@paintdream.com)
// 2015-5-5
//

#ifndef __FLUTELEAVES_H__
#define __FLUTELEAVES_H__

#include "../../Core/Interface/IScript.h"

namespace PaintsNow {
	namespace NsFluteLeaves {
		class FluteLeaves : public IScript::Library {
		public:
			FluteLeaves(IThread& thread);
		};
	}
}


#endif // __FLUTELEAVES_H__