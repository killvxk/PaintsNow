// SpaceComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-10
//

#ifndef __SPACECOMPONENT_H__
#define __SPACECOMPONENT_H__

#include "../../Component.h"

namespace PaintsNow {
	namespace NsMythForest {
		class Entity;
		class SpaceComponent : public Component {
		public:
			SpaceComponent();
			virtual ~SpaceComponent();

			enum {
				SPACECOMPONENT_LOCALIZED_THREAD_CONTEXT = COMPONENT_CUSTOM_BEGIN,
				SPACECOMPONENT_ATTACHED = COMPONENT_CUSTOM_BEGIN << 1,
				SPACECOMPONENT_CUSTOM_BEGIN = COMPONENT_CUSTOM_BEGIN << 2
			};

			virtual TObject<IReflect>& operator () (IReflect& reflect);
			void QueueRoutine(Engine& engine, ITask* task);
			void QueueQuickRoutine(Engine& engine, ITask* task);
			void RoutineQueryEntities(IScript::Request& request, const ZFloat3Pair& box, IScript::Request::Ref callback, uint32_t groupCount);
			bool Insert(Engine& engine, Entity* entity);
			bool Remove(Engine& engine, Entity* entity);
			void RemoveAll(Engine& engine);

			Entity* GetRootEntity() const;
			void SetRootEntity(Entity* entity);
			ZMatrixFloat4x4 GetWorldTransform() const;
			void SetWorldTransform(const ZMatrixFloat4x4& mat);

		protected:
			virtual void Initialize(Engine& engine, Entity* entity) override;
			virtual void Uninitialize(Engine& engine, Entity* entity) override;

			void UpdateEntityThreadIndex(Entity* entity);
			void FastRemoveNode(Engine& engine, Entity*& entity);
			bool ValidateEntity(Entity* entity);

		protected:
			Entity* rootEntity;
			ZMatrixFloat3x4 worldTransformCache;
		};
	}
}

#endif // __SPACECOMPONENT_H__