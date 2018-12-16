// ShapeComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2015-5-5
//

#ifndef __SHAPECOMPONENT_H__
#define __SHAPECOMPONENT_H__

#include "../../Component.h"
#include "../../../SnowyStream/SnowyStream.h"

namespace PaintsNow {
	namespace NsMythForest {
		class ShapeComponent : public Component {
		public:
			ShapeComponent();
			virtual ~ShapeComponent();
			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};
	}
}


#endif // __SHAPECOMPONENT_H__