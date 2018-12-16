// Annotation.h
// By PaintDream (paintdream@paintdream.com)
// 2018-2-10
//

#ifndef __ANNOTATION_H__
#define __ANNOTATION_H__

#include "../LostDream.h"

namespace PaintsNow {
	namespace NsLostDream {
		class Annotation : public LostDream::Qualifier {
		public:
			virtual bool Initialize();
			virtual bool Run(int randomSeed, int length);
			virtual void Summary();

			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};
	}
}

#endif // __ANNOTATION_H__
