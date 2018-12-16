#include "Listener.h"
#include "Connection.h"

using namespace PaintsNow;
using namespace PaintsNow::NsEchoLegend;
using namespace PaintsNow::NsBridgeSunset;

Listener::Listener(BridgeSunset& bs, INetwork& nt, WorkDispatcher* disp, IScript::Request::Ref peventHandler, IScript::Request::Ref pcallback, IScript::Request::Ref pconnectCallback, const String& ip, bool h, bool mode) : network(nt), bridgeSunset(bs), listener(nullptr), dispatcher(disp), httpd(nullptr), eventHandler(peventHandler), callback(pcallback), connectCallback(pconnectCallback), http(h), activated(false), packetMode(mode) {
	listener = network.OpenListener(dispatcher->GetDispatcher(), Wrap(this, &Listener::OnEvent), Wrap(this, &Listener::OnAccept), ip);
	if (listener != nullptr) {
		if (http) {
			httpd = network.OpenHttpd(listener, Wrap(this, &Listener::OnAcceptHttp));
		}
	}
}

bool Listener::IsValid() const {
	return listener != nullptr && (!http || httpd != nullptr);
}

void Listener::GetInfo(IScript::Request& request, IScript::Request::Ref callback) {
	String info;
	network.GetListenerInfo(listener, info);

	request.DoLock();
	request.Push();
	request.Call(Sync, callback, info);
	request.Pop();
	request.Dereference(callback);
	request.UnLock();
}

void Listener::OnEvent(INetwork::EVENT event) {
	bridgeSunset.Dispatch(WrapTaskScript(eventHandler, delegate(this), Looper::EventToString(event)));
}

Listener::~Listener() {
	if (httpd != nullptr) {
		network.CloseHttpd(httpd);
	}

	network.CloseListener(listener);
}

const TWrapper<void, INetwork::EVENT> Listener::OnAccept(INetwork::Connection* connection) {
	IScript::Request& r = bridgeSunset.GetScript().GetDefaultRequest();
	r.DoLock();
	IScript::Request::Ref ref = r.MakeReference(connectCallback);
	r.UnLock();

	WorkDispatcher* disp = dispatcher();
	INetwork::HttpRequest* req = (INetwork::HttpRequest*)nullptr;
	TShared<Connection> c = TShared<Connection>::Make(bridgeSunset, network, disp, ref, "", connection, false, req, false, packetMode);

	IScript::BaseDelegate d(this);
	IScript::BaseDelegate dc(c());
	bridgeSunset.Dispatch(WrapTaskScript(callback, delegate(this), delegate(c())));
	return Wrap(c(), &Connection::OnEvent);
}

void Listener::OnAcceptHttp(INetwork::Connection* connection, INetwork::HttpRequest* httpRequest) {
	IScript::Request& r = bridgeSunset.GetScript().GetDefaultRequest();
	r.DoLock();
	IScript::Request::Ref ref = r.MakeReference(connectCallback);
	r.UnLock();

	WorkDispatcher* disp = dispatcher();
	TShared<Connection> c = TShared<Connection>::Make(bridgeSunset, network, disp, ref, "", connection, true, httpRequest, true, packetMode);

	bridgeSunset.Dispatch(WrapTaskScript(callback, delegate(this), delegate(c())));
}


void Listener::ScriptUninitialize(IScript::Request& request) {
	request.DoLock();
	if (callback) {
		request.Dereference(callback);
	}

	if (eventHandler) {
		request.Dereference(eventHandler);
	}

	if (connectCallback) {
		request.Dereference(connectCallback);
	}
	request.UnLock();

	SharedTiny::ScriptUninitialize(request);
}

bool Listener::Activate() {
	bool ret = false;
	if (!activated) {
		activated = true;
		if (http) {
			ret = network.ActivateHttpd(httpd);
		} else {
			ret = network.ActivateListener(listener);
		}
		activated = false;
	}

	return ret;
}

void Listener::Deactivate() {
	if (activated) {
		network.DeactivateListener(listener);
		activated = false;
	}
}

TObject<IReflect>& Listener::operator () (IReflect& reflect) {
	ReflectClass(Listener);

	return *this;
}
