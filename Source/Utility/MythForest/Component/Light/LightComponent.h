// LightComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __LIGHTCOMPONENT_H__
#define __LIGHTCOMPONENT_H__

#include "../../Entity.h"
#include "../../Component.h"

namespace PaintsNow {
	namespace NsMythForest {
		class LightComponent : public Component {
		public:
			LightComponent();
			enum {
				LIGHTCOMPONENT_PARALLEL = COMPONENT_CUSTOM_BEGIN,
				LIGHTCOMPONENT_CUSTOM_BEGIN = COMPONENT_CUSTOM_BEGIN << 1
			};

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual FLAG GetEntityMask() const override final;

			ZFloat4 color; // RGB color with eccentricity
		};
	}
}


#endif // __LIGHTCOMPONENT_H__
