// Serialization.h
// By PaintDream (paintdream@paintdream.com)
// 2018-2-10
//

#ifndef __SERIALIZATION_H__
#define __SERIALIZATION_H__

#include "../LostDream.h"

namespace PaintsNow {
	namespace NsLostDream {
		class Serialization : public LostDream::Qualifier {
		public:
			virtual bool Initialize();
			virtual bool Run(int randomSeed, int length);
			virtual void Summary();

			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};
	}
}

#endif // __SERIALIZATION_H__
