// Component.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-4
//

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Module.h"
#include "Unit.h"
#include "Event.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class ResourceManager;
	}

	namespace NsMythForest {
		class Entity;
		class Component : public Unit {
		public:
			enum {
				COMPONENT_CUSTOM_BEGIN = UNIT_CUSTOM_BEGIN
			};
			
			TObject<IReflect>& operator () (IReflect& reflect);
			typedef NsSnowyStream::ResourceManager ResourceManager;

			virtual void Initialize(Engine& engine, Entity* entity);
			virtual void Uninitialize(Engine& engine, Entity* entity);
			virtual void DispatchEvent(Event& event);
			virtual FLAG GetEntityMask() const;
		};
	}
}

#endif // __COMPONENT_H__