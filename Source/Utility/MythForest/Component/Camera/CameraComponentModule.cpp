#include "CameraComponentModule.h"
#include "CameraComponent.h"
#include "../../Entity.h"
#include "../RenderFlow/RenderFlowComponent.h"
#include "../Event/EventListenerComponentModule.h"
#include "../../Engine.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsSnowyStream;

CameraComponentModule::CameraComponentModule(Engine& engine) : ModuleImpl<CameraComponent>(engine) {}

TObject<IReflect>& CameraComponentModule::operator () (IReflect& reflect) {
	ReflectClass(CameraComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestSetSymbol)[ScriptMethod = "SetSymbol"];
		ReflectMethod(RequestBindRootEntity)[ScriptMethod = "BindRootEntity"];
		ReflectMethod(RequestComputeCastRayFromPoint)[ScriptMethod = "ComputeCastRayFromPoint"];
		ReflectMethod(RequestSetPerspective)[ScriptMethod = "SetPerspective"];
		ReflectMethod(RequestGetPerspective)[ScriptMethod = "GetPerspective"];
		ReflectMethod(RequestSetVisibleDistance)[ScriptMethod = "SetVisibleDistance"];
		ReflectMethod(RequestGetVisibleDistance)[ScriptMethod = "GetVisibleDistance"];
	}

	return *this;
}

// Interfaces
void CameraComponentModule::RequestCreate(IScript::Request& request, IScript::Delegate<RenderFlowComponent> renderFlowComponent) {
	METHOD_ENTRY(CameraComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(renderFlowComponent);

	RenderFlowComponent* flow = renderFlowComponent.Get();
	TShared<CameraComponent> cameraComponent = TShared<CameraComponent>::Make(flow);

	request.DoLock();
	request << delegate(cameraComponent());
	request.UnLock();
}

void CameraComponentModule::RequestSetSymbol(IScript::Request& request, IScript::Delegate<CameraComponent> cameraComponent, String& inputSymbol, String& lightSymbol, String& outputSymbol) {
	METHOD_ENTRY(CameraComponentModule::RequestBindRootEntity);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(cameraComponent);
	CHECK_THREAD(cameraComponent);

	cameraComponent->symbolInputPrimitives = inputSymbol;
	cameraComponent->symbolInputLights = lightSymbol;
	cameraComponent->symbolOutputColor = outputSymbol;
}

void CameraComponentModule::RequestBindRootEntity(IScript::Request& request, IScript::Delegate<CameraComponent> camera, IScript::Delegate<Entity> entity) {
	METHOD_ENTRY(CameraComponentModule::RequestBindRootEntity);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(camera);
	CHECK_DELEGATE(entity);
	CHECK_THREAD(camera);

	camera->BindRootEntity(engine, entity.Get());
}

void CameraComponentModule::RequestSetPerspective(IScript::Request& request, IScript::Delegate<CameraComponent> camera, float d, float n, float f, float r) {
	METHOD_ENTRY(CameraComponentModule::RequestSetPerspective);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(camera);
	CHECK_THREAD(camera);

	camera->fov = d;
	camera->nearPlane = n;
	camera->farPlane = f;
	camera->ratio = r;
}

void CameraComponentModule::RequestGetPerspective(IScript::Request& request, IScript::Delegate<CameraComponent> camera) {
	METHOD_ENTRY(CameraComponentModule::RequestGetPerspective);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(camera);
	CHECK_THREAD(camera);

	request.DoLock();
	request << begintable
		<< key("Fov") << camera->fov
		<< key("Near") << camera->nearPlane
		<< key("Far") << camera->farPlane
		<< key("Ratio") << camera->ratio
		<< endtable;
	request.UnLock();
}

void CameraComponentModule::RequestSetVisibleDistance(IScript::Request& request, IScript::Delegate<CameraComponent> camera, float distance) {
	METHOD_ENTRY(CameraComponentModule::RequestSetVisibleDistance);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(camera);
	CHECK_THREAD(camera);

	camera->viewDistance = distance;
}

void CameraComponentModule::RequestGetVisibleDistance(IScript::Request& request, IScript::Delegate<CameraComponent> camera) {
	METHOD_ENTRY(CameraComponentModule::RequestGetVisibleDistance);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(camera);
	CHECK_THREAD(camera);

	request.DoLock();
	request << camera->viewDistance;
	request.UnLock();
}

// Functions
void CameraComponentModule::RequestComputeCastRayFromPoint(IScript::Request& request, IScript::Delegate<CameraComponent> camera, ZFloat2& point) {
	// TODO:
}
