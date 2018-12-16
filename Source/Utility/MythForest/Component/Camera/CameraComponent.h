// CameraComponent.h
// By PaintDream (paintdream@paintdream.com)
// 2018-1-19
//

#ifndef __CAMERACOMPONENT_H__
#define __CAMERACOMPONENT_H__

#include "../../Entity.h"
#include "../../Component.h"
#include "../../../SnowyStream/Resource/RenderTargetResource.h"
#include "../RenderFlow/RenderFlowComponent.h"
#include "../RenderFlow/RenderPort/RenderPortCommandQueue.h"
#include "../RenderFlow/RenderPort/RenderPortLightSource.h"
#include "../Event/EventListenerComponent.h"
#include <queue>

namespace PaintsNow {
	namespace NsMythForest {
		class BatchComponent;
		class LightComponent;
		class SpaceComponent;
		class RenderableComponent;
		class CameraComponent : public Component {
		public:
			enum {
				CAMERACOMPONENT_PERSPECTIVE = COMPONENT_CUSTOM_BEGIN,
				CAMERACOMPONENT_CUSTOM_BEGIN = COMPONENT_CUSTOM_BEGIN << 1
			};

			CameraComponent(TShared<RenderFlowComponent> renderFlowComponent);
			virtual void Initialize(Engine& engine, Entity* entity) override;
			virtual void Uninitialize(Engine& engine, Entity* entity) override;
			virtual Tiny::FLAG GetEntityMask() const override;

			void UpdateMatrices(const ZMatrixFloat4x4& cameraWorldMatrix);
			void BindRootEntity(Engine& engine, Entity* entity);
			Entity* GetRootEntity() const;
			bool IsVisible(const Box& box) const;
			RenderFlowComponent* GetRenderFlowComponent() const;

			float nearPlane;
			float farPlane;
			float fov;
			float ratio;
			float viewDistance;
			String symbolInputPrimitives;
			String symbolInputLights;
			String symbolOutputColor;

		protected:
			// collected cache
			typedef RenderPortCommandQueue::RenderableElement RenderableElement;
			typedef RenderPortLightSource::LightElement LightElement;

			virtual void DispatchEvent(Event& event);
			void OnTickRoot(Engine& engine);
			void OnTickHost(Engine& engine);
			void OnRender(Engine& engine);

			struct Collection : public SharedTiny {
				Collection(Engine& engine);
				virtual ~Collection();
				Engine& engine;
				IThread::Lock* lock;
				TShared<Entity> rootEntity;
				std::queue<RenderableElement> renderElements;
				std::vector<LightElement> lightElements;
				std::set<TShared<BatchComponent> > updatingBatchComponents;
				std::set<TShared<BatchComponent> > updatedBatchComponents;
				TAtomic<uint32_t> remaining;
			};

			void RoutineCollectRenderComponents(TShared<Collection> collection, const RenderableElement& derivedElement, TShared<Entity> rootEntity) const;
			void RoutineCollectRenderComponentsForSpaceComponent(TShared<Collection> collection, RenderableElement& derivedElement, SpaceComponent* spaceComponent) const;

		protected:
			// Collections
			TShared<Collection> currentCollection;
			TShared<Collection> nextCollection;

			// Runtime Resource
			TShared<RenderFlowComponent> renderFlowComponent;
			TShared<EventListenerComponent> rootEventListenerComponent;
			Entity* hostEntity;
			Entity* rootEntity;

			// Internal Data structure
			ZMatrixFloat4x4 viewProjectionMatrix;
			ZFloat4 planes[6];
			ZFloat3 orthoSize;
		};
	}
}


#endif // __CAMERACOMPONENT_H__
