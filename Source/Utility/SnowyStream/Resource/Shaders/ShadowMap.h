// ShadowMap.h
// By PaintDream (paintdream@paintdream.com)
// 2018-4-13
//

#ifndef __SHADOWMAP_H__
#define __SHADOWMAP_H__

#include "../../../../Core/Interface/IShader.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class ShadowMap : public IShader {
		public:
			ShadowMap();
			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};
	}
}


#endif // __SHADOWMAP_H__
