#include "CameraComponent.h"
#include "../Batch/BatchComponent.h"
#include "../Space/SpaceComponent.h"
#include "../Transform/TransformComponent.h"
#include "../Renderable/RenderableComponent.h"
#include "../RenderFlow/RenderFlowComponent.h"
#include "../RenderFlow/RenderPort/RenderPortRenderTarget.h"
#include "../RenderFlow/RenderPort/RenderPortLightSource.h"
#include "../RenderFlow/RenderPort/RenderPortCommandQueue.h"
#include "../Light/LightComponent.h"
#include "../../Engine.h"
#include "../../../BridgeSunset/BridgeSunset.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::NsBridgeSunset;

// From glu source code
const double PI = 3.14159265358979323846;

static void Ortho(ZMatrixFloat4x4& transform, const ZFloat3& size) {
	float f[16] = { 1.0f / size.x(), 0, 0, 0, 0, 1.0f / size.y(), 0, 0, 0, 0, 1.0f / size.z(), 0, 0, 0, 0.0f, 1.0f };
	transform = ZMatrixFloat4x4(f);
}

static void Perspective(ZMatrixFloat4x4& transform, float d, float rr, float n, float f) {
	float t = n * (float)tan(d * PI / 360);
	float b = -n * (float)tan(d * PI / 360);
	float r = rr * t;
	float l = rr * b;
	float trans[16] = { 2 * n / (r - l), 0, 0, 0, 0, 2 * n / (t - b), 0, 0, (r + l) / (r - l), (t + b) / (t - b), -(f + n) / (f - n), -1, 0, 0, -2 * f * n / (f - n), 0 };
	const ZMatrixFloat4x4 src = transform;
	transform = src * ZMatrixFloat4x4(trans);
}

static ZFloat4 BuildPlane(const ZFloat3& a, const ZFloat3& b, const ZFloat3& c) {
	ZFloat3 u = a - b;
	ZFloat3 v = c - b;
	ZFloat3 n = CrossProduct(u, v).Normalize();
	return ZFloat4(n.x(), n.y(), n.z(), DotProduct(a, n));
}

CameraComponent::CameraComponent(TShared<RenderFlowComponent> prenderFlowComponent) : fov(60.0f), ratio(16 / 9.0f), nearPlane(0.05f), farPlane(1000.0f), viewDistance(256), hostEntity(nullptr), rootEntity(nullptr), renderFlowComponent(prenderFlowComponent), symbolInputPrimitives("Primitives"), symbolInputLights("Lights"), symbolOutputColor("Screen") {}

void CameraComponent::UpdateMatrices(const ZMatrixFloat4x4& cameraWorldMatrix) {
	ZMatrixFloat4x4 projectionMatrix;

	if (Flag() & CAMERACOMPONENT_PERSPECTIVE) {
		Perspective(projectionMatrix, fov, ratio, nearPlane, farPlane);
	} else {
		Ortho(projectionMatrix, orthoSize);
	}

	Flag() &= ~TINY_MODIFIED;
	viewProjectionMatrix = projectionMatrix * cameraWorldMatrix;

	ZFloat3 position(-cameraWorldMatrix(3, 0), -cameraWorldMatrix(3, 1), -cameraWorldMatrix(3, 2));
	ZFloat3 up(cameraWorldMatrix(1, 0), cameraWorldMatrix(1, 1), cameraWorldMatrix(1, 2));
	ZFloat3 direction(-cameraWorldMatrix(2, 0), -cameraWorldMatrix(2, 1), -cameraWorldMatrix(2, 2));
	direction.Normalize();
	up.Normalize();

	// update planes ...
	ZFloat3 basisX = CrossProduct(direction, up).Normalize();
	ZFloat3 basisY = CrossProduct(basisX, direction);
	float v = (float)tan(fov * PI / 360 * 0.5f);
	float nearStepY = nearPlane * v;
	float nearStepX = nearStepY * ratio;
	float farStepY = farPlane * v;
	float farStepX = farStepY * ratio;

	ZFloat3 nearCenter = position + direction * nearPlane;
	ZFloat3 farCenter = position + direction * farPlane;

	ZFloat3 nearLeftBottom = nearCenter - basisX * nearStepX - basisY * nearStepY;
	ZFloat3 nearRightBottom = nearCenter + basisX * nearStepX - basisY * nearStepY;
	ZFloat3 nearLeftTop = nearCenter - basisX * nearStepX + basisY * nearStepY;
	ZFloat3 nearRightTop = nearCenter + basisX * nearStepX + basisY * nearStepY;

	ZFloat3 farLeftBottom = farCenter - basisX * farStepX - basisY * farStepY;
	ZFloat3 farRightBottom = farCenter + basisX * farStepX - basisY * farStepY;
	ZFloat3 farLeftTop = farCenter - basisX * farStepX + basisY * farStepY;
	ZFloat3 farRightTop = farCenter + basisX * farStepX + basisY * farStepY;

	// top
	planes[0] = BuildPlane(nearLeftTop, nearRightTop, farLeftTop);
	// bottom
	planes[1] = BuildPlane(nearLeftBottom, farLeftBottom, nearRightBottom);
	// left
	planes[2] = BuildPlane(nearLeftTop, farLeftTop, farLeftBottom);
	planes[3] = BuildPlane(nearRightTop, farRightBottom, farRightTop);
	// front
	planes[4] = BuildPlane(nearLeftTop, nearLeftBottom, nearRightBottom);
	planes[5] = BuildPlane(farLeftTop, farRightBottom, farLeftBottom);
}

bool CameraComponent::IsVisible(const Box& box) const {
	// check visibility
	ZFloat3 size = box.second - box.first;
	ZFloat3 center = box.first + size * 0.5f;
	for (size_t i = 0; i < sizeof(planes) / sizeof(planes[0]); i++) {
		const ZFloat4& plane = planes[i];
		float r = fabs(size.x() * plane.x()) + fabs(size.y() * plane.y()) + fabs(size.z() * plane.z());
		ZFloat3 n(plane.x(), plane.y(), plane.z());
		float d = DotProduct(n, center) + plane.w();
		float s = d < 0 ? d + r : d - r;
		if (s < 0) return false;
	}

	return true;
}

RenderFlowComponent* CameraComponent::GetRenderFlowComponent() const {
	return renderFlowComponent();
}

Tiny::FLAG CameraComponent::GetEntityMask() const {
	return Entity::ENTITY_HAS_CAMERA | Entity::ENTITY_HAS_TICK_EVENT;
}

void CameraComponent::Initialize(Engine& engine, Entity* entity) {
	if (rootEntity != entity) {
		Component::Initialize(engine, entity);
		hostEntity = entity;
	
		currentCollection = TShared<Collection>::Make(engine);
		nextCollection = TShared<Collection>::Make(engine);
	}
}

void CameraComponent::Uninitialize(Engine& engine, Entity* entity) {
	if (hostEntity == entity) {
		hostEntity = nullptr;
	}

	if (rootEntity == entity) {
		rootEntity = nullptr;
	}

	Component::Uninitialize(engine, entity);
}

// Event Dispatcher
void CameraComponent::DispatchEvent(Event& event) {
	// Invoke frame render
	if (event.eventID == Event::EVENT_FRAME) {
		OnRender(event.engine);
	}

	if (event.eventID == Event::EVENT_TICK && event.eventSender) {
		if (event.eventSender == hostEntity) {
			OnTickHost(event.engine);
		}

		if (event.eventSender == rootEntity) {
			OnTickRoot(event.engine);
		}
	}
}

void CameraComponent::OnTickHost(Engine& engine) {}

void CameraComponent::OnTickRoot(Engine& engine) {
	Entity* entity = rootEntity;
	if (entity != nullptr && (entity->Flag() & Entity::ENTITY_HAS_SPACE)) {
		RenderableElement baseSettings;

		// load finished
		if (nextCollection->remaining.load(std::memory_order_acquire) == 0) {
			TShared<Collection> collection = currentCollection;
			currentCollection = nextCollection;
			nextCollection = collection;

			++collection->remaining;

			collection->rootEntity = entity;
			const Entity::ComponentVector& components = entity->GetComponents();
			// Must called from entity thread
			assert(engine.GetCurrentWarpIndex() == entity->GetWarpIndex());
			static Unique uniqueSpace = UniqueType<SpaceComponent>::Get();

			for (size_t i = 0; i < components.size(); i++) {
				Component* component = components[i];
				if (component->GetEntityMask() & Entity::ENTITY_HAS_SPACE) {
					RoutineCollectRenderComponentsForSpaceComponent(collection, baseSettings, static_cast<SpaceComponent*>(component));
				}
			}

			--collection->remaining;
		}
	}
}

void CameraComponent::RoutineCollectRenderComponentsForSpaceComponent(TShared<Collection> collection, RenderableElement& derivedElement, SpaceComponent* spaceComponent) const {
	derivedElement.worldTransform = spaceComponent->GetWorldTransform();
	TShared<Entity> spaceRoot = spaceComponent->GetRootEntity();
	if (spaceRoot) {
		TShared<BatchComponent> originBatchComponent = derivedElement.batchComponent;
		TShared<BatchComponent> batchComponent = rootEntity->GetUniqueComponent(UniqueType<BatchComponent>());
		if (batchComponent && batchComponent != originBatchComponent) {
			if (batchComponent->Flag() & TINY_MODIFIED) {
				collection->updatingBatchComponents.insert(batchComponent);
			} else {
				collection->updatedBatchComponents.insert(batchComponent);
			}

			derivedElement.batchComponent = batchComponent;
		}

		++collection->remaining;
		if (spaceComponent->Flag() & SpaceComponent::SPACECOMPONENT_LOCALIZED_THREAD_CONTEXT) {
			Engine& engine = collection->engine;
			spaceComponent->QueueQuickRoutine(engine, WrapTaskQuickRoutine(Wrap(this, &CameraComponent::RoutineCollectRenderComponents), collection, (RenderableElement)derivedElement, spaceRoot()));
			engine.CommitRoutines();
		} else {
			RoutineCollectRenderComponents(collection, derivedElement, spaceComponent->GetRootEntity());
		}

		derivedElement.batchComponent = originBatchComponent;
	}
}

void CameraComponent::OnRender(Engine& engine) {
	if (renderFlowComponent) {
		RenderStage::Port* inputPort = renderFlowComponent->ImportSymbol(symbolInputPrimitives);
		RenderStage::Port* lightPort = renderFlowComponent->ImportSymbol(symbolInputLights);
		RenderStage::Port* outputPort = renderFlowComponent->ImportSymbol(symbolOutputColor);
		if (inputPort != nullptr && outputPort != nullptr && lightPort != nullptr) {
			// Check port compatibility.
			RenderPortCommandQueue* portCommandQueue = inputPort->QueryInterface(UniqueType<RenderPortCommandQueue>());
			RenderPortLightSource* portLightSource = lightPort->QueryInterface(UniqueType<RenderPortLightSource>());
			RenderPortRenderTarget* portRenderTarget = outputPort->QueryInterface(UniqueType<RenderPortRenderTarget>());

			if (portCommandQueue != nullptr && portRenderTarget != nullptr) {
				std::queue<RenderableElement>& renderableElements = currentCollection->renderElements;
				std::vector<LightElement>& lightElements = currentCollection->lightElements;
				std::set<TShared<BatchComponent> >& updatingBatchComponents = currentCollection->updatingBatchComponents;
				std::set<TShared<BatchComponent> >& updatedBatchComponents = currentCollection->updatedBatchComponents;

				Flag() &= ~Tiny::TINY_MODIFIED;

				// Process Lights
				portLightSource->UpdateLight(lightElements);

				// Process Primtives
				IRender::Target* renderTarget = portCommandQueue->BeginTransaction();
				IRender& render = engine.interfaces.render;

				static RenderableComponent::Instance unitInstance;

				for (std::set<TShared<BatchComponent> >::iterator ic = updatedBatchComponents.begin(); ic != updatedBatchComponents.end(); ++ic) {
					(*ic)->RenderInstance(render, renderTarget, unitInstance); // BatchComponent has already batched instance data
				}

				for (std::set<TShared<BatchComponent> >::iterator it = updatingBatchComponents.begin(); it != updatingBatchComponents.end(); ++it) {
					(*it)->BeginCapture(render);
				}

				BatchComponent* lastBatchComponent = nullptr;
				while (!renderableElements.empty()) {
					RenderableElement& element = renderableElements.front();
					RenderableComponent* component = element.renderableComponent();
					BatchComponent* batchComponent = element.batchComponent();

					// new batch set?
					if (lastBatchComponent != nullptr && (batchComponent != lastBatchComponent)) {
						lastBatchComponent->FlushCapture(render);
					}

					if (batchComponent != nullptr && (batchComponent->Flag() & TINY_UPDATING)) {
						batchComponent->DoCapture(render, component);
					}

					component->RenderInstance(render, renderTarget, element);
					renderableElements.pop();

					lastBatchComponent = batchComponent;
				}

				if (lastBatchComponent != nullptr) {
					lastBatchComponent->FlushCapture(render);
				}

				for (std::set<TShared<BatchComponent> >::iterator ip = updatingBatchComponents.begin(); ip != updatingBatchComponents.end(); ++ip) {
					(*ip)->BeginCapture(render);
				}

				portCommandQueue->EndTransaction();
			}
		}
	}
}

void CameraComponent::BindRootEntity(Engine& engine, Entity* entity) {
	if (rootEntity != nullptr) {
		// free last listener
		rootEntity->RemoveComponent(engine, this);
	}

	rootEntity = entity;
	if (entity != nullptr) {
		entity->AddComponent(engine, this);
	}
}

Entity* CameraComponent::GetRootEntity() const {
	return rootEntity;
}

CameraComponent::Collection::Collection(Engine& e) : engine(e) {
	lock = engine.interfaces.thread.NewLock();
	remaining.store(0, std::memory_order_relaxed);
}

CameraComponent::Collection::~Collection() {
	engine.interfaces.thread.DeleteLock(lock);
}

void CameraComponent::RoutineCollectRenderComponents(TShared<Collection> collection, const RenderableElement& derived, TShared<Entity> entity) const {
	static Unique uniqueSpace = UniqueType<SpaceComponent>::Get();

	if (entity) {
		Tiny::FLAG rootFlag = rootEntity->Flag().load(std::memory_order_acquire);
		const Box& box = rootEntity->GetKey();
		if ((rootFlag & Tiny::TINY_ACTIVATED) && IsVisible(box)) {
			std::queue<RenderableElement>& renderElements = collection->renderElements;
			std::vector<LightElement>& lightElements = collection->lightElements;

			RenderableElement subDerived = derived;

			// do not transform base entity
			assert(entity != collection->rootEntity);
			// has TransformComponent?
			TransformComponent* transformComponent = rootEntity->GetUniqueComponent(UniqueType<TransformComponent>());

			if (transformComponent != nullptr) {
				const ZMatrixFloat4x4& localTransform = transformComponent->GetLocalTransform();
				subDerived.worldTransform = localTransform * derived.worldTransform;
			}

			if (rootFlag & Entity::ENTITY_HAS_RENDERABLE) {
				const Entity::ComponentVector& components = rootEntity->GetComponents();
				
				for (size_t i = 0; i < components.size(); i++) {
					Component* component = components[i];
					Unique unique = component->GetUnique();

					// Since EntityMask would be much more faster than Reflection
					// We asserted that flaged components must be derived from specified implementations
					Tiny::FLAG entityMask = component->GetEntityMask();
					if (!(component->Flag() & Tiny::TINY_ACTIVATED)) continue;

					if (entityMask & Entity::ENTITY_HAS_SPACE) {
						RoutineCollectRenderComponentsForSpaceComponent(collection, subDerived, static_cast<SpaceComponent*>(component));
					} else if (entityMask & Entity::ENTITY_HAS_RENDERABLE) {
						RenderableComponent* renderableComponent = static_cast<RenderableComponent*>(component);
						if (renderableComponent->UpdateRenderData()) {
							RenderableElement element = subDerived;
							if (!element.batchComponent || (element.batchComponent->Flag() & TINY_MODIFIED)) {
								element.renderableComponent = renderableComponent;
								renderElements.push(std::move(element));
							}
						}
					} else if (entityMask & Entity::ENTITY_HAS_LIGHT) {
						LightComponent* lightComponent = static_cast<LightComponent*>(component);
						LightElement element;
						ZMatrixFloat4x4& worldTransform = subDerived.worldTransform;
						element.position = ZFloat3(-worldTransform(3, 0), -worldTransform(3, 1), -worldTransform(3, 2));
						element.direction = ZFloat3(-worldTransform(0, 2), -worldTransform(1, 2), -worldTransform(2, 2));
						lightElements.push_back(std::move(element));
					} else if (component == static_cast<const Component*>(this) && rootEntity == hostEntity) {
						CameraComponent* cameraComponent = static_cast<CameraComponent*>(component);
						// Trick: update transform internally
						cameraComponent->UpdateMatrices(subDerived.worldTransform);
					}
				}
			}

			assert(entity != collection->rootEntity);
			RoutineCollectRenderComponents(collection, subDerived, entity->Left());
			RoutineCollectRenderComponents(collection, subDerived, entity->Right());
		}
	}

	--collection->remaining;
}
