#include "EchoLegend.h"
#include "Listener.h"
#include "Connection.h"

using namespace PaintsNow;
using namespace PaintsNow::NsEchoLegend;
using namespace PaintsNow::NsBridgeSunset;

EchoLegend::EchoLegend(IThread& threadApi, INetwork& n, BridgeSunset& bs) : IScript::Library(threadApi), network(n), bridgeSunset(bs) {}

void EchoLegend::ScriptUninitialize(IScript::Request& request) {
	// TODO: Deactive all activive connections/listeners.
	Library::ScriptUninitialize(request);
}

TObject<IReflect>& EchoLegend::operator () (IReflect& reflect) {
	ReflectClass(EchoLegend)[Interface(IScript::Library)];
	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestOpenListener)[ScriptMethod = "OpenListener"];
		ReflectMethod(RequestActivateListener)[ScriptMethod = "ActivateListener"];
		ReflectMethod(RequestGetListenerInfo)[ScriptMethod = "GetListenerInfo"];
		ReflectMethod(RequestDeactivateListener)[ScriptMethod = "DeactivateListener"];
		ReflectMethod(RequestOpenConnection)[ScriptMethod = "OpenConnection"];
		ReflectMethod(RequestActivateConnection)[ScriptMethod = "ActivateConnection"];
		ReflectMethod(RequestDeactivateConnection)[ScriptMethod = "DeactivateConnection"];
		ReflectMethod(RequestGetConnectionInfo)[ScriptMethod = "GetConnectionInfo"];
		ReflectMethod(RequestWriteConnection)[ScriptMethod = "WriteConnection"];
		ReflectMethod(RequestReadConnection)[ScriptMethod = "ReadConnection"];
		ReflectMethod(RequestWriteConnectionHttpRequest)[ScriptMethod = "WriteConnectionHttpRequest"];
		ReflectMethod(RequestWriteConnectionHttpResponse)[ScriptMethod = "WriteConnectionHttpResponse"];
		ReflectMethod(RequestReadConnectionHttpRequest)[ScriptMethod = "ReadConnectionHttpRequest"];
		ReflectMethod(RequestParseURL)[ScriptMethod = "ParseURL"];
		ReflectMethod(RequestMakeURL)[ScriptMethod = "MakeURL"];
		ReflectMethod(RequestOpenDispatcher)[ScriptMethod = "OpenDispatcher"];
		ReflectMethod(RequestActivateDispatcher)[ScriptMethod = "ActivateDispatcher"];
		ReflectMethod(RequestDeactivateDispatcher)[ScriptMethod = "DeactivateDispatcher"];
	}

	return *this;
}


void EchoLegend::RequestOpenListener(IScript::Request& request, IScript::Delegate<WorkDispatcher> dispatcher, String& ip, bool http, IScript::Request::Ref eventHandler, IScript::Request::Ref callback, IScript::Request::Ref connectCallback, bool packetMode) {
	METHOD_ENTRY(EchoLegend::RequestOpenListener);
	CHECK_REFERENCES_WITH_TYPE(eventHandler U connectCallback U callback, IScript::Request::FUNCTION U IScript::Request::FUNCTION U IScript::Request::FUNCTION);
	CHECK_DELEGATE(dispatcher);

	WorkDispatcher* disp = dispatcher.Get();
	TShared<Listener> p = TShared<Listener>::Make(bridgeSunset, network, disp, eventHandler, callback, connectCallback, ip, http, packetMode);

	if (p->IsValid()) {
		request.DoLock();
		request << delegate(p());
		request.UnLock();
	}
}

void EchoLegend::RequestActivateListener(IScript::Request& request, IScript::Delegate<Listener> listener) {
	METHOD_ENTRY(EchoLegend::RequestActivateListener);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(listener);
	listener->Activate();
}

void EchoLegend::RequestDeactivateListener(IScript::Request& request, IScript::Delegate<Listener> listener) {
	METHOD_ENTRY(EchoLegend::RequestDeactivateListener);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(listener);
	listener->Deactivate();
}

void EchoLegend::RequestGetListenerInfo(IScript::Request& request, IScript::Delegate<Listener> listener, IScript::Request::Ref callback) {
	METHOD_ENTRY(EchoLegend::RequestGetListenerInfo);
	CHECK_REFERENCES(callback);
	CHECK_DELEGATE(listener);
	listener->GetInfo(request, callback);
}

void EchoLegend::RequestOpenConnection(IScript::Request& request, IScript::Delegate<WorkDispatcher> dispatcher, String& ip, bool http, IScript::Request::Ref connectCallback, bool packetMode) {
	METHOD_ENTRY(EchoLegend::RequestOpenConnection);
	CHECK_REFERENCES_WITH_TYPE(connectCallback, IScript::Request::FUNCTION);
	CHECK_DELEGATE(dispatcher);

	WorkDispatcher* disp = dispatcher.Get();
	ITunnel::Connection* conn = nullptr;
	INetwork::HttpRequest* req = nullptr;
	TShared<Connection> c = TShared<Connection>::Make(bridgeSunset, network, disp, connectCallback, ip, conn, http, req, false, packetMode);

	if (c->IsValid()) {
		request.DoLock();
		request << delegate(c());
		request.UnLock();
	}
}

void EchoLegend::RequestActivateConnection(IScript::Request& request, IScript::Delegate<Connection> connection) {
	METHOD_ENTRY(EchoLegend::RequestActivateConnection);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(connection);
	connection->Activate();
}

void EchoLegend::RequestDeactivateConnection(IScript::Request& request, IScript::Delegate<Connection> connection) {
	METHOD_ENTRY(EchoLegend::RequestDeactivateConnection);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(connection);
	connection->Deactivate();
}

void EchoLegend::RequestGetConnectionInfo(IScript::Request& request, IScript::Delegate<Connection> connection, IScript::Request::Ref callback) {
	METHOD_ENTRY(EchoLegend::RequestGetConnectionInfo);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(connection);
	connection->GetInfo(request, callback);
}

void EchoLegend::RequestWriteConnection(IScript::Request& request, IScript::Delegate<Connection> connection, String& data) {
	METHOD_ENTRY(EchoLegend::RequestWriteConnection);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(connection);
	connection->Write(request, data);
}

void EchoLegend::RequestReadConnection(IScript::Request& request, IScript::Delegate<Connection> connection, IScript::Request::Ref callback) {
	METHOD_ENTRY(EchoLegend::RequestReadConnection);
	CHECK_REFERENCES_WITH_TYPE(callback, IScript::Request::FUNCTION);
	CHECK_DELEGATE(connection);

	connection->Read(request, callback);
}

void EchoLegend::RequestWriteConnectionHttpRequest(IScript::Request& request, IScript::Delegate<Connection> connection, String& uri, String& method, std::list<std::pair<String, String> >& header, String& data) {
	METHOD_ENTRY(EchoLegend::RequestWriteConnectionHttpRequest);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(connection);
	connection->WriteHttpRequest(uri, method, header, data);
}

void EchoLegend::RequestWriteConnectionHttpResponse(IScript::Request& request, IScript::Delegate<Connection> connection, int code, String& data, String& reason, std::list<std::pair<String, String> >& header) {
	if (connection) {
		connection->WriteHttpResponse(data, code, reason, header);
	} else {
		request.Error("EchoLegend::WriteConnectionHttpReponse(connection, data) : invalid connection");
	}
}


void EchoLegend::RequestReadConnectionHttpRequest(IScript::Request& request, IScript::Delegate<Connection> connection, IScript::Request::Ref callback) {
	if (connection) {
		connection->ReadHttpRequest(request, callback);
	} else {
		request.Error("EchoLegend::ReadConnectionHttp(connection, data) : invalid connection");
		request.DoLock();
		request.Dereference(callback);
		request.UnLock();
	}
}

void EchoLegend::RequestParseURL(IScript::Request& request, String& url) {
	String user, host, path, fragment;
	int port;
	std::list<std::pair<String, String> > query;
	network.ParseUri(url, user, host, port, path, query, fragment);

	request.DoLock();
	request << begintable
		<< key("User") << user
		<< key("Host") << host
		<< key("Path") << path
		<< key("Fragment") << fragment
		<< key("Port") << port
		<< key("Query") << begintable;

	for (std::list<std::pair<String, String> >::const_iterator p = query.begin(); p != query.end(); ++p) {
		request << key((*p).first.c_str()) << (*p).second;
	}

	request << endtable << endtable;
	request.UnLock();
}

void EchoLegend::RequestMakeURL(IScript::Request& request, String& user, String& host, String& path, std::list<std::pair<String, String> >& query, String& fragment) {
	String result = network.MakeUri(user, host, path, query, fragment);
	request.DoLock();
	request << result;
	request.UnLock();
}

void EchoLegend::RequestOpenDispatcher(IScript::Request& request) {
	METHOD_ENTRY(EchoLegend::RequestOpenDispatcher);
	ITunnel::Dispatcher* disp = network.OpenDispatcher();
	if (disp != nullptr) {
		TShared<WorkDispatcher> dispatcher = TShared<WorkDispatcher>::Make(bridgeSunset, network, disp);
		request.DoLock();
		request << delegate(dispatcher());
		request.UnLock();
	}
}

void EchoLegend::RequestActivateDispatcher(IScript::Request& request, IScript::Delegate<WorkDispatcher> dispatcher) {
	METHOD_ENTRY(EchoLegend::RequestActivateDispatcher);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(dispatcher);

	dispatcher->AsyncActivate(request);
}

void EchoLegend::RequestDeactivateDispatcher(IScript::Request& request, IScript::Delegate<WorkDispatcher> dispatcher) {
	METHOD_ENTRY(EchoLegend::RequestActivateDispatcher);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(dispatcher);

	dispatcher->Deactivate();
}