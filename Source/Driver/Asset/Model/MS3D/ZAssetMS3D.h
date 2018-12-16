// ZAssetMS3D.h
// By PaintDream (paintdream@paintdream.com)
// 2014-12-25
//

#ifndef __ZASSETMS3D_H__
#define __ZASSETMS3D_H__

#include "../../../../Core/Interface/IAsset.h"

namespace PaintsNow {
	class ZAssetMS3D final : public IAsset::Model {
	public:
		virtual bool operator >> (IStreamBase& stream) const;
		virtual bool operator << (IStreamBase& stream);

		virtual TObject<IReflect>& operator () (IReflect& reflect);
	};
}


#endif // __ZASSETMS3D_H__