// RenderPortRenderTarget.h
// PaintDream (paintdream@paintdream.com)
// 2018-10-15
//

#ifndef __RENDERPORTRENDERTARGET_H__
#define __RENDERPORTRENDERTARGET_H__

#include "../RenderPort.h"

namespace PaintsNow {
	namespace NsMythForest {
		class RenderPortRenderTarget : public RenderPort {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect) override;
			TShared<NsSnowyStream::TextureResource> renderTargetTextureResource;
		};
	}
}

#endif // __RENDERPORTRENDERTARGET_H__