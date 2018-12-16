// StandardSkinning.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __STANDARDSKINNING_H__
#define __STANDARDSKINNING_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class StandardSkinning : public IShader {
		public:
			StandardSkinning();
			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};
	}
}


#endif // __STANDARDSKINNING_H__
