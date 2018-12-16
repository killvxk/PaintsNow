#include "Weaver.h"

using namespace PaintsNow;
using namespace PaintsNow::NsGalaxyWeaver;
using namespace PaintsNow::NsBridgeSunset;
using namespace PaintsNow::NsSnowyStream;

#define WEAVER_VERSION "2018.1.25"

Weaver::Weaver(BridgeSunset& sunset, SnowyStream& ns, ITunnel& tunnel, const String& entry, const TWrapper<void, const String&>& cw) : bridgeSunset(sunset), snowyStream(ns), ProxyStub(sunset.GetThreadApi(), tunnel, entry), consoleWriter(cw), started(false) {

}


void Weaver::ScriptUninitialize(IScript::Request& request) {
	IScript* script = request.GetScript();
	if (script == &bridgeSunset.GetScript()) {
		request.DoLock();
		request.Dereference(rpcCallback);
		request.Dereference(connectionCallback);
		request.UnLock();
	}
	
	SharedTiny::ScriptUninitialize(request);
}

static void ReplaceCallback(IScript::Request& request, IScript::Request::Ref& target, const IScript::Request::Ref& ref) {
	if (target) {
		request.DoLock();
		request.Dereference(target);
		request.UnLock();
	}

	target = ref;
}

void Weaver::SetRpcCallback(IScript::Request& request, const IScript::Request::Ref& ref) {
	ReplaceCallback(request, rpcCallback, ref);
}

void Weaver::SetConnectionCallback(IScript::Request& request, const IScript::Request::Ref& ref) {
	ReplaceCallback(request, connectionCallback, ref);
}

void Weaver::OnConnectionStatus(IScript::Request& request, bool isAuto, ZRemoteProxy::STATUS status, const String & message) {
	if (status == ZRemoteProxy::CONNECTED || status == ZRemoteProxy::CLOSED || status == ZRemoteProxy::ABORTED) {
		String code = status == ZRemoteProxy::CONNECTED ? "Connected" : status == ZRemoteProxy::CLOSED ? "Closed" : "Aborted";
		if (connectionCallback) {
			request.DoLock();
			request.Push();
			request.Call(Sync, connectionCallback, code, message);
			request.Pop();
			request.UnLock();
		}
	}
	/*
	if (status == ZRemoteProxy::CLOSED || status == ZRemoteProxy::ABORTED) {
		// restart if not manually stopped
		if (started) {
			started = false;
			Start();
		}
	}*/
}

void Weaver::Start() {
	if (started) {
		Stop();
	}

	remoteProxy.Run();
	started = true;
}

void Weaver::Stop() {
	started = false;
	remoteProxy.Reset();
}

TObject<IReflect>& Weaver::operator () (IReflect& reflect) {
	ReflectClass(Weaver);

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RpcCheckVersion)[ScriptMethod = "CheckVersion"];
		ReflectMethod(RpcPostResource)[ScriptMethod = "PostResource"];
		ReflectMethod(RpcCleanupFlyingResources)[ScriptMethod = "CleanupFlyingResources"];
		//	ReflectMethod(RpcPostCamera)[ScriptMethod = "PostCamera"];
	}

	return *this;
}

void Weaver::FilterRpcCall(const String& operation, const String& message) {
	// Make callback
	if (rpcCallback) {
		bridgeSunset.Dispatch(WrapTaskScript(rpcCallback, String(operation), (String)message));
	}
}

void Weaver::RpcCheckVersion(IScript::Request& request) {
	METHOD_ENTRY(Weaver::RpcCheckVersion);
	// Write current version string to output buffer.
	request.DoLock();
	request << WEAVER_VERSION;
	request.UnLock();

	// Make callback
	FilterRpcCall(METHOD_NAME(), WEAVER_VERSION);
}

void Weaver::RpcCleanupFlyingResources(IScript::Request& request, String& extension) {
	METHOD_ENTRY(Weaver::RpcCleanupFlyingResources);

	for (size_t i = 0; i < flyingResources.size(); i++) {
		flyingResources[i]->ReleaseObject();
	}

	flyingResources.clear();
	FilterRpcCall(METHOD_NAME(), extension);
}

void Weaver::RpcPostResource(IScript::Request& request, bool onfly, String& path, String& extension, String& resourceData) {
	METHOD_ENTRY(Weaver::RpcPostResource);
	// Make memory stream for deserialization
	size_t length = resourceData.length();
	ZMemoryStream memoryStream(length);
	memoryStream.Write(resourceData.c_str(), length);
	assert(length == resourceData.length());

	// Create resource from memory
	ResourceBase* resource = snowyStream.CreateResource(extension, path, false, &memoryStream);
	bool success = false;

	if (resource != nullptr) {
		if (onfly) {
			flyingResources.push_back(resource); // hold references
			success = true;
		} else {
			// Serialize it to disk
			success = snowyStream.PersistResource(resource);
 			// Finished, release reference now.
			resource->ReleaseObject();
		}
	}

	request.DoLock();
	request << success;
	request.UnLock();

	String message = path + extension;
	if (onfly) message += " [OnTheFly]"; // add flying mode notation
	message += success ? " [Succeed]" : " [Failed]";
	FilterRpcCall(METHOD_NAME(), path + extension);
}

void Weaver::RpcPostEntity(IScript::Request& request, bool onfly, String& entityData) {
	METHOD_ENTRY(Weaver::RpcPostEntity);

}

void Weaver::RpcUpdateView(IScript::Request& request, String& cameraSettings) {
	METHOD_ENTRY(Weaver::RpcUpdateView);
}

void Weaver::RpcUpdateEntity(IScript::Request& request, String& guid, String& entityData) {
	METHOD_ENTRY(Weaver::RpcUpdateEntity);

}

void Weaver::RpcInitialize(IScript::Request& request, String& clientVersion) {
	METHOD_ENTRY(Weaver::RpcInitialize);

}

void Weaver::RpcUninitialize(IScript::Request& request) {
	METHOD_ENTRY(Weaver::RpcDebugPrint);

}

void Weaver::RpcDebugPrint(IScript::Request& request, String& text) {
	METHOD_ENTRY(Weaver::RpcDebugPrint);
	
	assert(consoleWriter);
	consoleWriter(text);
}