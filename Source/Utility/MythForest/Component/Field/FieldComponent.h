// FieldComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2015-5-5
//

#ifndef __FIELDCOMPONENT_H__
#define __FIELDCOMPONENT_H__

#include "../../Component.h"
#include "../../../SnowyStream/SnowyStream.h"

namespace PaintsNow {
	namespace NsMythForest {
		class FieldComponent : public Component {
		public:
			FieldComponent();
			virtual ~FieldComponent();
			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};
	}
}


#endif // __FIELDCOMPONENT_H__