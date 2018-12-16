// ZWinPipe.h
// By PaintDream (paintdream@paintdream.com)
// 2016-7-5
//

// Windows pipe vs. Unix sockestd::pair

#ifndef __ZWINPIPE_H__
#define __ZWINPIPE_H__

#ifdef _WIN32

#include "../../../Core/Interface/ITunnel.h"
#include <Windows.h>

namespace PaintsNow {
	class ZWinPipe final : public ITunnel {
	public:
		ZWinPipe(size_t threadCount = 1, size_t bufferSize = 4096);
		virtual ~ZWinPipe();
		virtual Dispatcher* OpenDispatcher();
		virtual bool ActivateDispatcher(Dispatcher* dispatcher);
		virtual void DeactivateDispatcher(Dispatcher* dispatcher);
		virtual Dispatcher* GetListenerDispatcher(Listener* listener);
		virtual Dispatcher* GetConnectionDispatcher(Connection* connection);
		virtual void CloseDispatcher(Dispatcher* dispatcher);

		virtual ITunnel::Listener* OpenListener(Dispatcher* dispatcher, const TWrapper<void, EVENT>& eventHandler, const TWrapper<const TWrapper<void, EVENT>, Connection*>& callback, const String& address);
		virtual ITunnel::Connection* OpenConnection(Dispatcher* dispatcher, const TWrapper<void, EVENT>& callback, const String& address);
		virtual ITunnel::Listener* QueryListeners(const String& filter);

		virtual bool ActivateListener(ITunnel::Listener* listener);
		virtual void GetListenerInfo(ITunnel::Listener* listener, String& info);
		virtual void DeactivateListener(ITunnel::Listener* listener);
		virtual void CloseListener(ITunnel::Listener* listener);
		virtual bool ActivateConnection(ITunnel::Connection* connection);
		virtual void DeactivateConnection(ITunnel::Connection* connection);
		virtual void CloseConnection(ITunnel::Connection* connection);
		virtual void GetConnectionInfo(ITunnel::Connection* connection, String& from, String& to);
		virtual void Flush(ITunnel::Connection* connection);
		virtual bool ReadConnection(ITunnel::Connection* connection, void* data, size_t& length);
		virtual bool WriteConnection(ITunnel::Connection* connection, const void* data, size_t& length);

	protected:
		size_t bufferSize;
		size_t threadCount;
	};
}

#endif

#endif // __ZWINPIPE_H__