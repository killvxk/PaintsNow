#include "Connection.h"

using namespace PaintsNow;
using namespace PaintsNow::NsEchoLegend;
using namespace PaintsNow::NsBridgeSunset;

namespace PaintsNow {
	IScript::Request& operator >> (IScript::Request& request, std::list<std::pair<String, String> >& mylist) {
		request >> begintable;
		std::vector<IScript::Request::Key> keys = request.Enumerate();
		for (size_t i = 0; i < keys.size(); i++) {
			String value;
			request >> keys[i];
			if (keys[i].GetType() == IScript::Request::STRING) {
				request >> value;
				mylist.push_back(std::make_pair(keys[i].GetKey(), value));
			} else if (keys[i].GetType() == IScript::Request::TABLE) {
				IScript::Request::TableStart ts;
				request >> ts;
				for (size_t i = 0; i < ts.count; i++) {
					request >> value;
					mylist.push_back(std::make_pair(keys[i].GetKey(), value));
				}
				request >> endtable;
			}
		}

		request >> endtable;
		return request;
	}

	IScript::Request& operator << (IScript::Request& request, const std::list<std::pair<String, String> >& mylist) {
		std::map<String, std::list<String> > data;
		for (std::list<std::pair<String, String> >::const_iterator q = mylist.begin(); q != mylist.end(); ++q) {
			data[(*q).first].push_back((*q).second);
		}

		request << begintable;
		for (std::map<String, std::list<String> >::const_iterator it = data.begin(); it != data.end(); ++it) {
			request << key(it->first.c_str());
			const std::list<String>& st = it->second;
			if (st.size() > 1) {
				request << begintable;
				for (std::list<String>::const_iterator p = st.begin(); p != st.end(); ++p) {
					request << *p;
				}
				request << endtable;
			} else if (st.size() == 1) {
				request << *st.begin();
			} else {
				assert(false);
			}
		}
		request << endtable;

		return request;
	}
}

Connection::Connection(BridgeSunset& bs, INetwork& nt, WorkDispatcher* disp, IScript::Request::Ref cb, const String& ip, INetwork::Connection* con, bool h, INetwork::HttpRequest* httpReq, bool ownReq, bool mode) : bridgeSunset(bs), network(nt),  dispatcher(disp), connection(con), httpRequest(httpReq), callback(cb), http(h), activated(false), ownConnection(false), ownRequest(ownReq), packetMode(mode) {
	if (connection == nullptr) {
		connection = network.OpenConnection(dispatcher->GetDispatcher(), Wrap(this, &Connection::OnEvent), ip);
		ownConnection = true;
	} else {
		activated = true;
	}

	if (connection != nullptr) {
		dispatcher->ReferenceObject();

		if (http) {
			if (httpRequest == nullptr) {
				httpRequest = network.OpenHttpRequest(connection, Wrap(this, &Connection::OnEventHttp));
				ownRequest = true;
			}
		}
	}
}

bool Connection::IsValid() const {
	return connection != nullptr && (!http || httpRequest != nullptr);
}

void Connection::Deactivate() {
	if (activated) {
		network.DeactivateConnection(connection);
		activated = false;
	}
}

Connection::~Connection() {
	if (httpRequest != nullptr && ownRequest) {
		network.CloseHttpRequest(httpRequest);
	}

	if (connection != nullptr && ownConnection) {
		network.CloseConnection(connection);
	}

	if (dispatcher != nullptr) {
		dispatcher->ReleaseObject();
	}
}

bool Connection::Activate() {
	bool ret = false;
	if (!activated) {
		activated = true;
		ret = network.ActivateConnection(connection);
	}

	return ret;
}


IScript::Request::Ref Connection::GetCallback() const {
	return callback;
}

void Connection::OnEventHttp(int code) {
	bridgeSunset.Dispatch(WrapTaskScript(callback, (int)code));
}

struct Header {
	uint32_t packetLength;
	uint32_t packetHash;
};

void Connection::OnEvent(INetwork::EVENT event) {
	if (event == INetwork::READ && packetMode) {
		String segment;
		INetwork::PacketSizeType blockSize = 0x1000;
		segment.resize(blockSize);
		while (network.ReadConnectionPacket(connection, const_cast<char*>(segment.data()), blockSize, currentState)) {
			// new packet?
			currentData.append(segment.data(), blockSize);
			if (currentState.cursor == currentState.header.length) {
				DispatchEvent(event);
				currentData.resize(0);
			}
		}
	} else {
		DispatchEvent(event);

		if (http && event == INetwork::CONNECTED) {
			OnEvent(INetwork::READ);
		}
	}
}

void Connection::DispatchEvent(INetwork::EVENT event) {
	if (event == INetwork::READ && packetMode) {
		bridgeSunset.Dispatch(WrapTaskScript(callback, Looper::EventToString(event), String(currentData)));
	} else {
		bridgeSunset.Dispatch(WrapTaskScript(callback, Looper::EventToString(event)));
	}
}

TObject<IReflect>& Connection::operator () (IReflect& reflect) {
	ReflectClass(Connection);

	if (reflect.IsReflectMethod()) {
		ReflectMethod(Activate);
		ReflectMethod(Deactivate);
	}

	return *this;
}

void Connection::ScriptUninitialize(IScript::Request& request) {
	if (callback) {
		request.DoLock();
		request.Dereference(callback);
		request.UnLock();
	}

	SharedTiny::ScriptUninitialize(request);
}

void Connection::Read(IScript::Request& request, IScript::Request::Ref callback) {
	if (!activated) {
		request.Error("EchoLegend::Connection::Read(callback) : Not an activated connection");
	} else if (!callback) {
		request.Error("EchoLegend::Connection::Read(callback) : Not a valid callback function");
	} else {
		size_t length = 0;
		network.ReadConnection(connection, nullptr, length);
		String data;
		data.resize(length);
		network.ReadConnection(connection, const_cast<char*>(data.data()), length);
		
		request.DoLock();
		request.Push();
		request.Call(Sync, callback, currentData);
		request.Pop();
		request.UnLock();
	}

	request.DoLock();
	request.Dereference(callback);
	request.UnLock();
}

void Connection::Write(IScript::Request& request, const String& data) {
	if (!activated) {
		request.Error("EchoLegend::Connection::Read(callback) : Not an activated connection");
	} else {
		if (packetMode) {
			INetwork::Packet packet;
			INetwork::PacketSizeType length = (INetwork::PacketSizeType)data.length();
			packet.header.length = length;
			network.WriteConnectionPacket(connection, data.data(), length, packet);
		} else {
			size_t length = data.length();
			network.WriteConnection(connection, data.data(), length);
		}
		network.Flush(connection);
	}
}

void Connection::GetInfo(IScript::Request& request, IScript::Request::Ref callback) {
	String src, dst;
	network.GetConnectionInfo(connection, src, dst);

	request.DoLock();
	request.Push();
	request.Call(Sync, callback, src, dst);
	request.Pop();
	request.Dereference(callback);
	request.UnLock();
}

void Connection::ReadHttpRequest(IScript::Request& request, IScript::Request::Ref callback) {
	if (httpRequest != nullptr) {
		std::list<std::pair<String, String> > header;
		network.GetHttpRequestHeader(httpRequest, header);
		String uri = network.GetHttpRequestUri(httpRequest);
		String method = network.GetHttpRequestMethod(httpRequest);
		String data = network.GetHttpRequestData(httpRequest);

		request.DoLock();
		request.Push();
		// request << uri << method << header << data;
		request.Call(Sync, callback, uri, method, header, data);
		request.Dereference(callback);
		request.Pop();
		request.UnLock();
	}
}

void Connection::WriteHttpRequest(const String& uri, const String& method, const std::list<std::pair<String, String> >& header, const String& data) {
	if (httpRequest != nullptr) {
		network.PrepareHttpRequest(httpRequest);
		network.SetHttpRequestMethod(httpRequest, method);
		network.SetHttpRequestUri(httpRequest, uri);
		network.SetHttpRequestHeader(httpRequest, header);
		network.SetHttpRequestData(httpRequest, data);
		network.MakeHttpRequest(httpRequest);
	}
}

void Connection::WriteHttpResponse(const String& data, int code, const String& reason, std::list<std::pair<String, String> >& header) {
	if (httpRequest != nullptr) {
		network.SetHttpRequestHeader(httpRequest, header);
		network.MakeHttpResponse(httpRequest, code, reason, data);
	}
}

