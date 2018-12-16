#include "RenderFlowComponentModule.h"

#include "RenderStage/DeferredLightingRenderStage.h"
#include "RenderStage/GeometryBufferRenderStage.h"
#include "RenderStage/HighDynamicRangeRenderStage.h"
#include "RenderStage/ScreenSpaceTraceRenderStage.h"
#include "RenderStage/ShadowMaskRenderStage.h"
#include "RenderStage/TemporalResamplerRenderStage.h"

using namespace PaintsNow;
using namespace PaintsNow::NsMythForest;

#define REGISTER_TEMPLATE(type) \
	TFactory<type, RenderStage> stage##type; \
	stageTemplates[#type] = stage##type;

RenderFlowComponentModule::RenderFlowComponentModule(Engine& engine) : ModuleImpl<RenderFlowComponent>(engine) {
	// Register built-ins
	REGISTER_TEMPLATE(DeferredLightingRenderStage);
	REGISTER_TEMPLATE(GeometryBufferRenderStage);
	REGISTER_TEMPLATE(HighDynamicRangeRenderStage);
	REGISTER_TEMPLATE(ScreenSpaceTraceRenderStage);
	REGISTER_TEMPLATE(ShadowMaskRenderStage);
	REGISTER_TEMPLATE(TemporalResamplerRenderStage);
}

TObject<IReflect>& RenderFlowComponentModule::operator () (IReflect& reflect) {
	ReflectClass(RenderFlowComponentModule)[Interface(Module)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreate)[ScriptMethod = "Create"];
		ReflectMethod(RequestCreateRenderStage)[ScriptMethod = "CreateRenderStage"];
		ReflectMethod(RequestEnumerateRenderStagePorts)[ScriptMethod = "EnumerateRenderStagePorts"];
		ReflectMethod(RequestConnectRenderStagePort)[ScriptMethod = "ConnectRenderStagePort"];
		ReflectMethod(RequestDisconnectRenderStagePort)[ScriptMethod = "DisconnectRenderStagePort"];
		ReflectMethod(RequestExportRenderStagePort)[ScriptMethod = "ExportRenderStagePort"];
		ReflectMethod(RequestDeleteRenderStage)[ScriptMethod = "DeleteRenderStage"];
	}

	return *this;
}

void RenderFlowComponentModule::RegisterNodeTemplate(const String& key, const TFactoryBase<RenderStage>& t) {
	stageTemplates[key] = t;
}

void RenderFlowComponentModule::RequestCreate(IScript::Request& request) {
	METHOD_ENTRY(RenderFlowComponentModule::RequestCreate);
	CHECK_REFERENCES_NONE();

	TShared<RenderFlowComponent> renderFlowComponent = TShared<RenderFlowComponent>::Make();
	request.DoLock();
	request << delegate(renderFlowComponent());
	request.UnLock();
}

void RenderFlowComponentModule::RequestCreateRenderStage(IScript::Request& request, IScript::Delegate<RenderFlowComponent> renderFlow, String& name, String& config) {
	METHOD_ENTRY(RenderFlowComponentModule::RequestCreateRenderStage);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(renderFlow);
	CHECK_THREAD(renderFlow);

	std::map<String, TFactoryBase<RenderStage> >::const_iterator it = stageTemplates.find(name);
	if (it != stageTemplates.end()) {
		RenderStage* renderStage = it->second(config);
		renderFlow->AddNode(renderStage);
		request.DoLock();
		request << delegate(renderStage);
		request.UnLock();
		renderStage->ReleaseObject();
	} else {
		request.Error(String("Could not find render stage: ") + name);
	}
}

void RenderFlowComponentModule::RequestEnumerateRenderStagePorts(IScript::Request& request,  IScript::Delegate<RenderStage> renderStage) {
	METHOD_ENTRY(RenderFlowComponentModule::RequestEnumerateRenderStagePorts);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(renderStage);

	request.DoLock();
	RenderStage* s = renderStage.Get();
	request << begintable;
	for (std::map<String, RenderStage::Port*>::const_iterator it = s->GetPortMap().begin(); it != s->GetPortMap().end(); ++it) {
		RenderStage::Port* port = it->second;
		request << begintable <<
			key("Name") << it->first <<
			key("Type") << port->GetUnique().info->typeName <<
			key("Targets") << begintable;

		for (std::map<RenderStage::Port*, Tiny::FLAG>::const_iterator ip = port->GetTargetPortMap().begin(); ip != port->GetTargetPortMap().end(); ++ip)	 {
			RenderStage::Port* targetPort = ip->first;
			RenderStage* target = static_cast<RenderStage*>(targetPort->GetNode());
			String targetPortName;
			// locate port
			for (std::map<String, RenderStage::Port*>::const_iterator iw = target->GetPortMap().begin(); iw != target->GetPortMap().end(); ++iw) {
				if (iw->second == targetPort) {
					targetPortName = iw->first;
					break;
				}
			}

			request << begintable
				<< key("RenderStage") << delegate(target)
				<< key("Port") << targetPortName
				<< key("Direction") << !!(ip->second)
				<< endtable;
		}

		request << endtable << endtable;
	}
	
	request << endtable;
	request.UnLock();
}

void RenderFlowComponentModule::RequestConnectRenderStagePort(IScript::Request& request, IScript::Delegate<RenderStage> from, const String& fromPortName, IScript::Delegate<RenderStage> to, const String& toPortName) {
	METHOD_ENTRY(RenderFlowComponentModule::RequestConnectRenderStagePort);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(from);
	CHECK_DELEGATE(to);

	RenderStage::Port* fromPort = (*from.Get())[fromPortName];
	RenderStage::Port* toPort = (*to.Get())[toPortName];

	if (fromPort == nullptr) {
		request.Error(String("Unable to locate port") + fromPortName);
		return;
	}

	if (toPort == nullptr) {
		request.Error(String("Unable to locate port") + toPortName);
		return;
	}

	// check compatibility
	if (toPort->GetUnique() != fromPort->GetUnique()) {
		request.Error(String("Port type not compatible") + fromPortName + " - " + toPortName);
		return;
	}
	
	fromPort->Link(toPort);
}

void RenderFlowComponentModule::RequestDisconnectRenderStagePort(IScript::Request& request, IScript::Delegate<RenderStage> from, const String& fromPortName, IScript::Delegate<RenderStage> to, const String& toPortName) {
	METHOD_ENTRY(RenderFlowComponentModule::RequestDisconnectRenderStagePort);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(from);
	CHECK_DELEGATE(to);

	RenderStage::Port* fromPort = (*from.Get())[fromPortName];
	RenderStage::Port* toPort = (*to.Get())[toPortName];

	if (fromPort == nullptr) {
		request.Error(String("Unable to locate port") + fromPortName);
		return;
	}

	if (toPort == nullptr) {
		request.Error(String("Unable to locate port") + toPortName);
		return;
	}
	
	fromPort->UnLink(toPort);
}

void RenderFlowComponentModule::RequestExportRenderStagePort(IScript::Request& request, IScript::Delegate<RenderFlowComponent> renderFlowComponent, String& symbol, IScript::Delegate<RenderStage> stage, String& port) {
	METHOD_ENTRY(RenderFlowComponentModule::RequestExportRenderStagePort);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(renderFlowComponent);
	CHECK_DELEGATE(stage);
	CHECK_THREAD(renderFlowComponent);

	renderFlowComponent->ExportSymbol(std::move(symbol), stage.Get(), port);
}

void RenderFlowComponentModule::RequestDeleteRenderStage(IScript::Request& request, IScript::Delegate<RenderFlowComponent> renderFlowComponent, IScript::Delegate<RenderStage> stage) {
	METHOD_ENTRY(RenderFlowComponentModule::RequestDeleteRenderStage);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(renderFlowComponent);
	CHECK_DELEGATE(stage);
	CHECK_THREAD(renderFlowComponent);

	renderFlowComponent->RemoveNode(stage.Get());
}