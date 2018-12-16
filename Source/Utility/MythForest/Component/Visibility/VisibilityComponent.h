// VisibilityComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __VISIBILITYCOMPONENT_H__
#define __VISIBILITYCOMPONENT_H__

#include "../../Entity.h"
#include "../../Component.h"

namespace PaintsNow {
	namespace NsMythForest {
		class VisibilityComponent : public Component {
		public:
			VisibilityComponent();
			enum {
				VISIBILITYCOMPONENT_PARALLEL = COMPONENT_CUSTOM_BEGIN,
				VISIBILITYCOMPONENT_CUSTOM_BEGIN = COMPONENT_CUSTOM_BEGIN << 1
			};

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual FLAG GetEntityMask() const override final;
		};
	}
}


#endif // __VISIBILITYCOMPONENT_H__
