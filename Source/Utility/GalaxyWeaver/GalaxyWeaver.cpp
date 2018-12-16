#include "GalaxyWeaver.h"

using namespace PaintsNow;
using namespace PaintsNow::NsGalaxyWeaver;
using namespace PaintsNow::NsBridgeSunset;
using namespace PaintsNow::NsMythForest;

GalaxyWeaver::GalaxyWeaver(IThread& threadApi, ITunnel& net, NsBridgeSunset::BridgeSunset& sunset, NsSnowyStream::SnowyStream& ns, const TWrapper<void, const String&>& cw) : Library(threadApi), network(net), bridgeSunset(sunset), snowyStream(ns), consoleWriter(cw) {}

TObject<IReflect>& GalaxyWeaver::operator () (IReflect& reflect) {
	ReflectClass(GalaxyWeaver)[Interface(IScript::Library)];

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreateWeaver)[ScriptMethod = "CreateWeaver"];
		ReflectMethod(RequestConnectEntity)[ScriptMethod = "ConnectEntity"];
		ReflectMethod(RequestSetWeaverRpcCallback)[ScriptMethod = "SetWeaverRpcCallback"];
		ReflectMethod(RequestSetWeaverConnectionCallback)[ScriptMethod = "SetWeaverConnectionCallback"];
		ReflectMethod(RequestStartWeaver)[ScriptMethod = "StartWeaver"];
		ReflectMethod(RequestStopWeaver)[ScriptMethod = "StopWeaver"];
		ReflectMethod(RequestCommitWeaverChanges)[ScriptMethod = "CommitWeaverChanges"];
	}

	return *this;
}

void GalaxyWeaver::RequestSetWeaverRpcCallback(IScript::Request& request, IScript::Delegate<Weaver> weaver, IScript::Request::Ref callback) {
	METHOD_ENTRY(GalaxyWeaver::RequestSetWeaverRpcCallback);
	CHECK_REFERENCES_WITH_TYPE(callback, IScript::Request::FUNCTION);
	CHECK_DELEGATE(weaver);

	weaver->SetRpcCallback(request, callback);
}

void GalaxyWeaver::RequestSetWeaverConnectionCallback(IScript::Request& request, IScript::Delegate<Weaver> weaver, IScript::Request::Ref callback) {
	METHOD_ENTRY(GalaxyWeaver::RequestSetWeaverConnectionCallback);
	CHECK_REFERENCES_WITH_TYPE(callback, IScript::Request::FUNCTION);
	CHECK_DELEGATE(weaver);

	weaver->SetConnectionCallback(request, callback);
}

void GalaxyWeaver::RequestCreateWeaver(IScript::Request& request, String& config) {
	METHOD_ENTRY(GalaxyWeaver::RequestCreateWeaver);
	
	TShared<Weaver> weaver = TShared<Weaver>::Make(bridgeSunset, snowyStream, network, config, consoleWriter);

	request.DoLock();
	request << delegate(weaver());
	request.UnLock();
}

void GalaxyWeaver::RequestStartWeaver(IScript::Request& request, IScript::Delegate<Weaver> weaver) {
	METHOD_ENTRY(GalaxyWeaver::RequestStartWeaver);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(weaver);

	weaver->Start();
}

void GalaxyWeaver::RequestStopWeaver(IScript::Request& request, IScript::Delegate<Weaver> weaver) {
	METHOD_ENTRY(GalaxyWeaver::RequestStopWeaver);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(weaver);

	weaver->Stop();

}

void GalaxyWeaver::RequestCommitWeaverChanges(IScript::Request& request, IScript::Delegate<Weaver> weaver) {
	METHOD_ENTRY(GalaxyWeaver::RequestCommitWeaverChanges);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(weaver);

}

void GalaxyWeaver::RequestConnectEntity(IScript::Request& request, IScript::Delegate<Weaver> weaver, IScript::Delegate<Entity> scene) {
	METHOD_ENTRY(GalaxyWeaver::RequestConnectEntity);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(weaver);
	CHECK_DELEGATE(scene);
}