// RenderPort.h
// PaintDream (paintdream@paintdream.com)
// 2018-9-24
//

#ifndef __RENDERPORT_H__
#define __RENDERPORT_H__

#include "../../../SnowyStream/Resource/TextureResource.h"
#include "../../../SnowyStream/Resource/RenderTargetResource.h"
#include "../../../SnowyStream/Resource/ShaderResource.h"

namespace PaintsNow {
	namespace NsMythForest {
		class RenderPort : public IReflectObjectComplex {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect) override;
			std::set<String> publicSymbols;
		};
	}
}


#endif // __RENDERPORT_H__