// FormComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-4
//

#ifndef __FORMCOMPONENT_H__
#define __FORMCOMPONENT_H__

#include "../../Entity.h"
#include "../../Component.h"
#include "../../../../Core/Common/ZType.h"

namespace PaintsNow {
	namespace NsMythForest {
		class FormComponent : public Component {
		public:
			FormComponent(const String& name);

			String name; // entity name, maybe
			std::vector<String> values;
		};
	}
}


#endif // __FORMCOMPONENT_H__