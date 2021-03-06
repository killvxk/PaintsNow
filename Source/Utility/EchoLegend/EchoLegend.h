// EchoLegend.h -- multi-player client/server support
// By PaintDream (paintdream@paintdream.com)
// 2015-5-29
// This project will be started at .... I don't know.
//

#ifndef __ECHOLEGEND_H__
#define __ECHOLEGEND_H__

#include "../../Core/Interface/IScript.h"
#include "../../Core/Interface/INetwork.h"
#include "../BridgeSunset/BridgeSunset.h"
#include "Connection.h"
#include "Listener.h"
#include "WorkDispatcher.h"

namespace PaintsNow {
	namespace NsEchoLegend {
		class EchoLegend : public IScript::Library {
		public:
			EchoLegend(IThread& threadApi, INetwork& network, NsBridgeSunset::BridgeSunset& b);
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual void ScriptUninitialize(IScript::Request& request);
			void RequestOpenDispatcher(IScript::Request& request);
			void RequestActivateDispatcher(IScript::Request& request, IScript::Delegate<WorkDispatcher> dispatcher);
			void RequestDeactivateDispatcher(IScript::Request& request, IScript::Delegate<WorkDispatcher> dispatcher);
			void RequestOpenListener(IScript::Request& request, IScript::Delegate<WorkDispatcher> dispatcher, String& ip, bool http, IScript::Request::Ref eventHandler, IScript::Request::Ref callback, IScript::Request::Ref connectCallback, bool packetMode);
			void RequestActivateListener(IScript::Request& request, IScript::Delegate<Listener> listener);
			void RequestGetListenerInfo(IScript::Request& request, IScript::Delegate<Listener> listener, IScript::Request::Ref callback);
			void RequestDeactivateListener(IScript::Request& request, IScript::Delegate<Listener> listener);
			void RequestOpenConnection(IScript::Request& request, IScript::Delegate<WorkDispatcher> dispatcher, String& ip, bool http, IScript::Request::Ref connectCallback, bool packetMode);
			void RequestActivateConnection(IScript::Request& request, IScript::Delegate<Connection> listener);
			void RequestDeactivateConnection(IScript::Request& request, IScript::Delegate<Connection> listener);
			void RequestGetConnectionInfo(IScript::Request& request, IScript::Delegate<Connection> connection, IScript::Request::Ref callback);
			void RequestWriteConnection(IScript::Request& request, IScript::Delegate<Connection> connection, String& data);
			void RequestReadConnection(IScript::Request& request, IScript::Delegate<Connection> connection, IScript::Request::Ref callback);
			void RequestWriteConnectionHttpRequest(IScript::Request& request, IScript::Delegate<Connection> connection, String& uri, String& method, std::list<std::pair<String, String> >& header, String& data);
			void RequestWriteConnectionHttpResponse(IScript::Request& request, IScript::Delegate<Connection> connection, int code, String& data, String& reason, std::list<std::pair<String, String> >& header);
			void RequestReadConnectionHttpRequest(IScript::Request& request, IScript::Delegate<Connection> connection, IScript::Request::Ref handler);
			void RequestParseURL(IScript::Request& request, String& input);
			void RequestMakeURL(IScript::Request& request, String& user, String& host, String& path, std::list<std::pair<String, String> >& query, String& fragment);

		protected:
			INetwork& network;
			NsBridgeSunset::BridgeSunset& bridgeSunset;
		};
	}
}

#endif // __ECHOLEGEND_H__
