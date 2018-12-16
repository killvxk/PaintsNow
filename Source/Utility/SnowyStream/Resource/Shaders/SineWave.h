// SineWave.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __SINEWAVE_H__
#define __SINEWAVE_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class SineWave : public IShader {
		public:
			SineWave();
			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};
	}
}


#endif // __SINEWAVE_H__
