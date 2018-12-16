#ifdef _WIN32
#include "ZWinPipe.h"
#include "../../../Core/Template/TAtomic.h"
#include <process.h>

using namespace PaintsNow;

const DWORD PIPE_TIMEOUT = 5000;

class WinPipeDispatcherImpl : public ITunnel::Dispatcher {
public:
	HANDLE iocp;
	CRITICAL_SECTION cs;
	std::vector<HANDLE> threads;

#ifdef _DEBUG
	TAtomic<DWORD> referCount;
#endif
	bool activated;
	bool deleting;
};

struct OverlappedBase : public OVERLAPPED {
	ITunnel::EVENT event;
	DWORD rva;
};

struct PipeBase {
	PipeBase(const TWrapper<void, ITunnel::EVENT>& cb, HANDLE pipe, WinPipeDispatcherImpl* disp, size_t bufferSize) : callback(cb), pipeHandle(pipe), dispatcher(disp), pendingReadPosition(0), pendingReadTotal(0) {
		memset(&readOverlap, 0, sizeof(readOverlap));
		memset(&writeOverlap, 0, sizeof(writeOverlap));
		memset(&connectOverlap, 0, sizeof(connectOverlap));
	
		readOverlap.event = ITunnel::READ;
		readOverlap.rva = offsetof(PipeBase, readOverlap);
		writeOverlap.event = ITunnel::WRITE;
		writeOverlap.rva = offsetof(PipeBase, writeOverlap);
		connectOverlap.event = ITunnel::CONNECTED;
		connectOverlap.rva = offsetof(PipeBase, connectOverlap);

		readBuffer.resize(bufferSize);
		activated = false;
		server = false;

		::InitializeCriticalSection(&cs);
	}

	~PipeBase() {
		::DeleteCriticalSection(&cs);
	}

	static PipeBase* FromOverlap(OVERLAPPED* overlapped, ITunnel::EVENT& event) {
		OverlappedBase* b = static_cast<OverlappedBase*>(overlapped);
		event = b->event;
		return reinterpret_cast<PipeBase*>(reinterpret_cast<char*>(b) - b->rva);
	}

	OverlappedBase readOverlap;
	OverlappedBase writeOverlap;
	OverlappedBase connectOverlap;
	CRITICAL_SECTION cs;
	size_t pendingReadPosition;
	size_t pendingReadTotal;
	String readBuffer;
	String writeBuffer;
	std::list<String> pendingWriteBuffers;
	TWrapper<void, ITunnel::EVENT> callback;
	HANDLE pipeHandle;
	WinPipeDispatcherImpl* dispatcher;
	bool activated;
	bool server;
};


struct WinPipeConnectionImpl : public PipeBase, public ITunnel::Connection {
	WinPipeConnectionImpl(const TWrapper<void, ITunnel::EVENT>& cb, HANDLE pipe, WinPipeDispatcherImpl* disp, size_t bufferSize, const String& e) : PipeBase(cb, pipe, disp, bufferSize), entry(e) {}
	String entry;
};

struct WinPipeListenerImpl : public WinPipeConnectionImpl, public ITunnel::Listener {
	WinPipeListenerImpl(const TWrapper<void, ITunnel::EVENT>& cb, HANDLE pipe, WinPipeDispatcherImpl* disp, size_t bufferSize, const String& e, const TWrapper<const TWrapper<void, ITunnel::EVENT>, ITunnel::Connection*>& cbc) : WinPipeConnectionImpl(TWrapper<void, ITunnel::EVENT>(), pipe, disp, bufferSize, e), serverConnectCallback(cbc), serverCallback(cb) {
		server = true;
	}

	const TWrapper<const TWrapper<void, ITunnel::EVENT>, ITunnel::Connection*> serverConnectCallback;
	TWrapper<void, ITunnel::EVENT> serverCallback;
};

ZWinPipe::ZWinPipe(size_t tc, size_t bc) : threadCount(tc), bufferSize(bc) {
	if (threadCount == 0) {
		// use core thread count
		SYSTEM_INFO sysInfo = { 0 };
		::GetSystemInfo(&sysInfo);

		threadCount = (size_t)sysInfo.dwNumberOfProcessors;
	}
}

ZWinPipe::~ZWinPipe() {}

ITunnel::Listener* ZWinPipe::OpenListener(Dispatcher* dispatcher, const TWrapper<void, EVENT>& eventHandler, const TWrapper<const TWrapper<void, EVENT>, Connection*>& callback, const String& entry) {
	assert(dispatcher != nullptr);
	WinPipeDispatcherImpl* disp = static_cast<WinPipeDispatcherImpl*>(dispatcher);
	String pipeName = String("\\\\.\\pipe\\") + entry;
	HANDLE pipeHandle = ::CreateNamedPipeA(pipeName.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE | PIPE_WAIT, 1, (DWORD)bufferSize, (DWORD)bufferSize, PIPE_TIMEOUT, nullptr);

	if (pipeHandle == INVALID_HANDLE_VALUE) {
		return nullptr;
	}

	HANDLE iocp = ::CreateIoCompletionPort(pipeHandle, disp->iocp, ITunnel::CUSTOM, (DWORD)disp->threads.size());
	if (iocp == nullptr) {
		::CloseHandle(pipeHandle);
		return nullptr;
	}

	WinPipeListenerImpl* listener = new WinPipeListenerImpl(eventHandler, pipeHandle, disp, bufferSize, entry, callback);

#ifdef _DEBUG
	disp->referCount++;
#endif
	return listener;
}

ITunnel::Listener* ZWinPipe::QueryListeners(const String& filter) {
	return nullptr;
}


void ZWinPipe::CloseListener(ITunnel::Listener* listener) {
	WinPipeListenerImpl* impl = static_cast<WinPipeListenerImpl*>(listener);

#ifdef _DEBUG
	impl->dispatcher->referCount--;
#endif
	::CloseHandle(impl->pipeHandle);
	delete impl;
}


static bool StartListener(WinPipeListenerImpl* impl) {
	if (!impl->activated) {
		impl->activated = true;
		::ConnectNamedPipe(impl->pipeHandle, &impl->connectOverlap);
		DWORD lastError = ::GetLastError();
		if (lastError == ERROR_PIPE_CONNECTED) { // already connected?
			impl->serverCallback(ITunnel::CONNECTED);
			::PostQueuedCompletionStatus(impl->dispatcher->iocp, 0, ITunnel::CONNECTED, &impl->connectOverlap);
			return true;
		} else {
			return false;
		}
	} else {
		return true;
	}
}


static bool ValidateResult(PipeBase* header, BOOL success) {
	if (!success && ::GetLastError() != ERROR_IO_PENDING) {
		if (header->callback) {
			header->callback(ITunnel::ABORT);
		}

		// relisten
		if (header->server) {
			StartListener(static_cast<WinPipeListenerImpl*>(header));
		}

		return false;
	} else {
		return true;
	}
}

bool ZWinPipe::ActivateListener(ITunnel::Listener* listener) {
	assert(listener != nullptr);
	WinPipeListenerImpl* impl = static_cast<WinPipeListenerImpl*>(listener);
	return StartListener(impl);
}

void ZWinPipe::GetListenerInfo(ITunnel::Listener* listener, String& info) {
	assert(listener != nullptr);
	WinPipeListenerImpl* impl = static_cast<WinPipeListenerImpl*>(listener);
	info = impl->entry;
}

void ZWinPipe::DeactivateListener(ITunnel::Listener* listener) {
	WinPipeListenerImpl* impl = static_cast<WinPipeListenerImpl*>(listener);
	if (impl->activated) {
		impl->serverCallback(ITunnel::CLOSE);
		::DisconnectNamedPipe(impl->pipeHandle);
		impl->activated = false;
	}
}

ITunnel::Connection* ZWinPipe::OpenConnection(Dispatcher* dispatcher, const TWrapper<void, EVENT>& callback, const String& entry) {
	assert(dispatcher != nullptr);
	WinPipeDispatcherImpl* disp = static_cast<WinPipeDispatcherImpl*>(dispatcher);
	String pipeName = String("\\\\.\\pipe\\") + entry;
	HANDLE pipeHandle = ::CreateFileA(pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
	if (pipeHandle == INVALID_HANDLE_VALUE) {
		return nullptr;
	}

	DWORD dwMode = PIPE_READMODE_BYTE;
	if (!::SetNamedPipeHandleState(pipeHandle, &dwMode, nullptr, nullptr)) {
		::CloseHandle(pipeHandle);
		return nullptr;
	}

	HANDLE iocp = ::CreateIoCompletionPort(pipeHandle, disp->iocp, ITunnel::CUSTOM, (DWORD)disp->threads.size());
	if (iocp == nullptr) {
		::CloseHandle(pipeHandle);
		return nullptr;
	}

	WinPipeConnectionImpl* impl = new WinPipeConnectionImpl(callback, pipeHandle, disp, bufferSize, entry);
#ifdef _DEBUG
	disp->referCount++;
#endif
	
	return impl;
}


bool ZWinPipe::ActivateConnection(ITunnel::Connection* connection) {
	// Post connection info
	assert(connection != nullptr);
	WinPipeConnectionImpl* impl = static_cast<WinPipeConnectionImpl*>(connection);
	if (!impl->activated) {
		impl->activated = true;
		// Write hello
		// static const char welcomeMessage[] = "Hello!";
		// ::WriteFile(impl->pipeHandle, welcomeMessage, sizeof(welcomeMessage), nullptr, &impl->writeOverlap);
		// ::FlushFileBuffers(impl->pipeHandle);
		// OK, Post connected
		::PostQueuedCompletionStatus(impl->dispatcher->iocp, 0, ITunnel::CONNECTED, &impl->connectOverlap);
	}
	return true;
}

void ZWinPipe::DeactivateConnection(ITunnel::Connection* connection) {
	// DO NOTHING HERE
	WinPipeConnectionImpl* impl = static_cast<WinPipeConnectionImpl*>(connection);
	if (impl->activated) {
		::PostQueuedCompletionStatus(impl->dispatcher->iocp, 0, ITunnel::CLOSE, &impl->connectOverlap);
		impl->activated = false;
	}
}

void ZWinPipe::CloseConnection(ITunnel::Connection* connection) {
	assert(connection != nullptr);
	WinPipeConnectionImpl* impl = static_cast<WinPipeConnectionImpl*>(connection);
	assert(impl->pipeHandle != nullptr);
	if (!impl->server) {
		::CloseHandle(impl->pipeHandle);
		impl->pipeHandle = nullptr;
#ifdef _DEBUG
		impl->dispatcher->referCount--;
#endif

		delete impl;
	}
}

void ZWinPipe::GetConnectionInfo(ITunnel::Connection* connection, String& from, String& to) {
	assert(connection != nullptr);
	WinPipeConnectionImpl* impl = static_cast<WinPipeConnectionImpl*>(connection);
	assert(impl->pipeHandle != nullptr);

	from = "local";
	to = impl->entry;
}

static bool ContinueWrite(WinPipeConnectionImpl* impl) {
	// write complete, remove it from pending buffer and write next
	bool result = true;
	if (!impl->pendingWriteBuffers.empty()) {
		std::swap(impl->writeBuffer, *impl->pendingWriteBuffers.begin());
		impl->pendingWriteBuffers.pop_front();
		if (!ValidateResult(impl, ::WriteFile(impl->pipeHandle, const_cast<char*>(impl->writeBuffer.data()), (DWORD)impl->writeBuffer.size(), nullptr, &impl->writeOverlap))) {
			result = false;
		}
	}

	return result;
}

void ZWinPipe::Flush(ITunnel::Connection* connection) {
	assert(connection != nullptr);
	WinPipeConnectionImpl* impl = static_cast<WinPipeConnectionImpl*>(connection);
	assert(impl->pipeHandle != nullptr);

	::EnterCriticalSection(&impl->cs);
	if (impl->writeBuffer.empty()) {
		ContinueWrite(impl);
	}
	::LeaveCriticalSection(&impl->cs);
}

bool ZWinPipe::ReadConnection(ITunnel::Connection* connection, void* data, size_t& length) {
	assert(connection != nullptr);
	WinPipeConnectionImpl* impl = static_cast<WinPipeConnectionImpl*>(connection);
	::EnterCriticalSection(&impl->cs);
	assert(impl->pipeHandle != nullptr);
	assert(impl->pendingReadTotal < impl->readBuffer.size());
	// get bytes avaliable?
	if (data == nullptr) {
		length = impl->pendingReadTotal - impl->pendingReadPosition;
		::LeaveCriticalSection(&impl->cs);
		return false;
	}

	bool ret = false;
	if (impl->pendingReadPosition < impl->pendingReadTotal) {
		length = Min(length, impl->pendingReadTotal - impl->pendingReadPosition);
		// do data fetching ...
		memcpy(data, impl->readBuffer.data() + impl->pendingReadPosition, length);
		impl->pendingReadPosition += length;
		ret = true;
	}

	assert(impl->pendingReadPosition <= impl->pendingReadTotal);
	if (impl->pendingReadPosition == impl->pendingReadTotal) {
		// Invoke next read call
		impl->pendingReadPosition = impl->pendingReadTotal = 0;
		if (!ValidateResult(impl, ::ReadFile(impl->pipeHandle, const_cast<char*>(impl->readBuffer.data()), (DWORD)impl->readBuffer.size(), nullptr, &impl->readOverlap))) {
			ret = false;
		}
	}

	::LeaveCriticalSection(&impl->cs);
	return ret;
}

bool ZWinPipe::WriteConnection(ITunnel::Connection* connection, const void* data, size_t& length) {
	assert(connection != nullptr);
	WinPipeConnectionImpl* impl = static_cast<WinPipeConnectionImpl*>(connection);
	assert(impl->pipeHandle != nullptr);
	assert(length != 0);
	bool ret = true;

	::EnterCriticalSection(&impl->cs);
	bool compacted = false;
	if (!impl->pendingWriteBuffers.empty()) {
		String& last = *impl->pendingWriteBuffers.rbegin();
		if (last.size() + length < bufferSize) {
			last.append((const char*)data, length);
			compacted = true;
		}
	}

	if (!compacted) {
		impl->pendingWriteBuffers.push_back(String((const char*)data, length));
	}
	::LeaveCriticalSection(&impl->cs);
	return true;
}

ITunnel::Dispatcher* ZWinPipe::OpenDispatcher() {
	DWORD tc = (DWORD)threadCount;
	HANDLE iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, tc);
	if (iocp == nullptr) {
		return nullptr;
	}

	WinPipeDispatcherImpl* impl = new WinPipeDispatcherImpl();
	impl->iocp = iocp;
	impl->threads.resize(tc);
	impl->activated = false;
	impl->deleting = false;
#ifdef _DEBUG
	impl->referCount = 0;
#endif

	return impl;
}

static void ThreadProc(void* param) {
	assert(param != nullptr);
	WinPipeDispatcherImpl* impl = reinterpret_cast<WinPipeDispatcherImpl*>(param);

	DWORD dwBytesTrans = 0;
	ULONG_PTR ulCompleteKey = 0;
	LPOVERLAPPED lpOverlapped = nullptr;

	impl->activated = true;
	while (::GetQueuedCompletionStatus(impl->iocp, &dwBytesTrans, &ulCompleteKey, &lpOverlapped, INFINITE)) {
		if (lpOverlapped == nullptr) {
			if (ulCompleteKey == ITunnel::ABORT || ulCompleteKey == ITunnel::CLOSE) {
				break;
			}
		} else {
			ITunnel::EVENT event;
			PipeBase* header = PipeBase::FromOverlap(lpOverlapped, event);
			if (ulCompleteKey != ITunnel::CUSTOM) {
				// parse std event from complete key
				event = (ITunnel::EVENT)ulCompleteKey;
			}
			
			::EnterCriticalSection(&header->cs);
			DWORD bytesTransferred;
			bool result = true;
			switch (event) {
				case ITunnel::CONNECTED:
					// invoke first read request ...
					if (header->server) {
						WinPipeListenerImpl* listener = static_cast<WinPipeListenerImpl*>(header);
						listener->callback = listener->serverConnectCallback(listener);
					}
				
					// post read request ...
					if (!ValidateResult(header, ::ReadFile(header->pipeHandle, const_cast<char*>(header->readBuffer.data()), (DWORD)header->readBuffer.size(), nullptr, &header->readOverlap))) {
						result = false;
					}

					break;
				case ITunnel::READ:
					if (!ValidateResult(header, ::GetOverlappedResult(header->pipeHandle, &header->readOverlap, &bytesTransferred, TRUE))) {
						result = false;
					} else {
						header->pendingReadTotal = bytesTransferred;
					}
					break;
				case ITunnel::WRITE:
					header->writeBuffer = ""; // clear last written data
					result = ContinueWrite(static_cast<WinPipeConnectionImpl*>(header));
					break;
				case ITunnel::CLOSE:
				case ITunnel::ABORT:
					header->pendingWriteBuffers.clear();
					break;
			}

			::LeaveCriticalSection(&header->cs);

			assert(header != nullptr);
			if (header->callback && result) {
				header->callback(event);
			}
		}
	}
}

bool ZWinPipe::ActivateDispatcher(Dispatcher* dispatcher) {
	assert(dispatcher != nullptr);
	WinPipeDispatcherImpl* impl = static_cast<WinPipeDispatcherImpl*>(dispatcher);
	// create threads ...
	if (impl->threads.size() <= 1) {
		// use current thread ...
		impl->threads[0] = nullptr;
		ThreadProc(impl);
	} else {
		// Create thread warps ...
		for (size_t i = 0; i < impl->threads.size(); i++) {
			// _beginthread returns actually HANDLE.
			impl->threads[i] = reinterpret_cast<HANDLE>(_beginthread(ThreadProc, 0, impl));
		}

		// join
		::WaitForMultipleObjects((DWORD)impl->threads.size(), &impl->threads[0], TRUE, INFINITE);

		for (size_t j = 0; j < impl->threads.size(); j++) {
			::CloseHandle(impl->threads[j]); // Was this really needed?
		}
	}

	impl->activated = false;

	if (impl->deleting) {
		delete impl;
	}
	return true;
}

void ZWinPipe::DeactivateDispatcher(Dispatcher* dispatcher) {
	assert(dispatcher != nullptr);
	WinPipeDispatcherImpl* impl = static_cast<WinPipeDispatcherImpl*>(dispatcher);
	for (size_t i = 0; i < impl->threads.size(); i++) {
		::PostQueuedCompletionStatus(impl->iocp, 0, ITunnel::CLOSE, nullptr);
	}
}

ITunnel::Dispatcher* ZWinPipe::GetListenerDispatcher(Listener* listener) {
	assert(listener != nullptr);
	WinPipeListenerImpl* impl = static_cast<WinPipeListenerImpl*>(listener);
	return impl->dispatcher;
}

ITunnel::Dispatcher* ZWinPipe::GetConnectionDispatcher(Connection* connection) {
	assert(connection != nullptr);
	WinPipeConnectionImpl* impl = static_cast<WinPipeConnectionImpl*>(connection);
	return impl->dispatcher;
}

void ZWinPipe::CloseDispatcher(Dispatcher* dispatcher) {
	assert(dispatcher != nullptr);
	WinPipeDispatcherImpl* impl = static_cast<WinPipeDispatcherImpl*>(dispatcher);
	HANDLE iocp = impl->iocp;

	impl->deleting = true;
	if (impl->activated) {
		for (size_t i = 0; i < impl->threads.size(); i++) {
			::PostQueuedCompletionStatus(impl->iocp, 0, ITunnel::CLOSE, nullptr);
		}
		
		// Wait for all-finished.
		::WaitForMultipleObjects((DWORD)impl->threads.size(), &impl->threads[0], TRUE, INFINITE);
		assert(impl->referCount.load() == 0);
	} else {
		delete impl;
	}

	::CloseHandle(iocp);
}

#endif