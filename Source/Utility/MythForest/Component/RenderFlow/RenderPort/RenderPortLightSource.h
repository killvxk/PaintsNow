// RenderPortLightSource.h
// PaintDream (paintdream@paintdream.com)
// 2018-10-15
//

#ifndef __RENDERPORTLIGHTSOURCE_H__
#define __RENDERPORTLIGHTSOURCE_H__

#include "../RenderPort.h"
#include "../../Light/LightComponent.h"
#include <queue>

namespace PaintsNow {
	namespace NsMythForest {
		class LightComponent;
		class RenderPortLightSource : public RenderPort {
		public:
			virtual TObject<IReflect>& operator () (IReflect& reflect) override;
			struct LightElement {
				ZFloat3 position;
				ZFloat3 direction;
				TShared<LightComponent> lightComponent;
			};

			void UpdateLight(std::vector<LightElement>& lightElements);

		protected:
			std::vector<LightElement> lightElements;
		};
	}
}

#endif // __RENDERPORTLIGHTSOURCE_H__