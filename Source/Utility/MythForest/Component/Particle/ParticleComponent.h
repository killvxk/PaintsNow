// ParticleComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2015-5-5
//

#ifndef __PARTICLECOMPONENT_H__
#define __PARTICLECOMPONENT_H__

#include "../../Component.h"
#include "../../../SnowyStream/SnowyStream.h"

namespace PaintsNow {
	namespace NsMythForest {
		class ParticleComponent : public Component {
		public:
			ParticleComponent();
			virtual ~ParticleComponent();
			virtual TObject<IReflect>& operator () (IReflect& reflect);
		};
	}
}


#endif // __PARTICLECOMPONENT_H__