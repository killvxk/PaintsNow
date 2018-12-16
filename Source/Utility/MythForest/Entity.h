// Entity.h
// By PaintDream (paintdream@paintdream.com)
// 2017-12-27
//

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "../../Core/Template/TKdTree.h"
#include "../../Core/Common/ZType.h"
#include "Unit.h"
#include "Module.h"
#include "Event.h"
#include <set>

namespace PaintsNow {
	namespace NsMythForest {
		enum { ENTITY_VISIT_CONST = 256 };
		class Component;
		class Engine;
		typedef TKdTree<Box, ENTITY_VISIT_CONST> Tree;

		// however 'object allocated on the heap may not be aligned 64'
		#if defined(_MSC_VER) && _MSC_VER <= 1200
		__declspec(align(64)) class Entity
		#else
		class alignas(64) Entity
		#endif
		: public Unit, public Tree {
		public:
			Entity();
			virtual ~Entity();

			enum {
				ENTITY_HAS_BEGIN = UNIT_CUSTOM_BEGIN,
				ENTITY_HAS_TICK_EVENT = ENTITY_HAS_BEGIN,
				ENTITY_HAS_TACH_EVENTS = ENTITY_HAS_BEGIN << 1,
				ENTITY_HAS_CUSTOM_EVENTS = ENTITY_HAS_BEGIN << 2,
				ENTITY_HAS_RENDERABLE = ENTITY_HAS_BEGIN << 3,
				ENTITY_HAS_LIGHT = ENTITY_HAS_BEGIN << 4,
				ENTITY_HAS_CAMERA = ENTITY_HAS_BEGIN << 5,
				ENTITY_HAS_SPACE = ENTITY_HAS_BEGIN << 6,
				ENTITY_HAS_END = ENTITY_HAS_BEGIN << 7,
				ENTITY_HAS_ENGINE = ENTITY_HAS_BEGIN << 8,
				ENTITY_HAS_ALL = ENTITY_HAS_END - ENTITY_HAS_BEGIN,
			};

			void AddComponent(Engine& engine, Component* component);
			void RemoveComponent(Engine& engine, Component* component);
			void ClearComponents(Engine& engine);

			void SetEngineInternal(Engine& engine);
			void CleanupEngineInternal();

			void UpdateEntityFlags();
			Component* GetUniqueComponent(Unique unique) const;
			void PostEvent(Event& event);

			typedef Vector<Component*> ComponentVector;
			const ComponentVector& GetComponents() const;
			template <class T>
			T* GetUniqueComponent(UniqueType<T> type) const {
				return static_cast<T*>(GetUniqueComponent(UniqueType<T>::Get()));
			}

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual Unit* Raycast(ZFloat3& intersection, ZFloat3& normal, const ZFloat3Pair& ray, IReflectObject* metaInfo) const;

			inline Entity* Left() const {
				return static_cast<Entity*>(leftNode);
			}

			inline Entity*& Left() {
				return reinterpret_cast<Entity*&>(leftNode);
			}

			inline Entity* Right() const {
				return static_cast<Entity*>(rightNode);
			}

			inline Entity*& Right() {
				return reinterpret_cast<Entity*&>(rightNode);
			}

		protected:
			Engine& GetEngineInternal() const;

		protected:
			ComponentVector components; // make whole entity object exactly 64 bytes on 32-bit platforms.
		};
	}
}

#endif // __ENTITY_H__