#ifdef _WIN32
#include "PipeMsg.h"
using namespace PaintsNow;
using namespace PaintsNow::NsLostDream;

PipeMsg::PipeMsg() : pipe(1, 1024), listener(nullptr), connection(nullptr), serverConnection(nullptr) {}
void PipeMsg::OnListenerEvent(ITunnel::EVENT event) {
	if (event == ITunnel::CONNECTED) {
		printf("[Server] Listener established.\n");
	}
}

void PipeMsg::OnServerEvent(ITunnel::EVENT event) {
	if (event == ITunnel::READ) {
		char message[MESSAGE_LENGTH] = "";
		size_t length = MESSAGE_LENGTH;
		pipe.ReadConnection(serverConnection, message, length);
		printf("[Server] Receive message: %s\n", message);
	}
}

void PipeMsg::OnClientEvent(ITunnel::EVENT event) {
	if (event == ITunnel::CONNECTED) {
		// write message
		char message[MESSAGE_LENGTH] = "Hello, WinPipe!";
		printf("[Client] Sending message: %s\n", message);
		size_t length = MESSAGE_LENGTH;
		pipe.WriteConnection(connection, message, length);
	}
}

const TWrapper<void, ITunnel::EVENT> PipeMsg::OnConnection(ITunnel::Connection* connection) {
	serverConnection = connection;
	return Wrap(this, &PipeMsg::OnServerEvent);
}

bool PipeMsg::Run(int randomSeed, int length) {
	return true;
	ITunnel::Dispatcher* dispatcher = pipe.OpenDispatcher();
	const String pipeName = "LostDreamTestPipe";
	listener = pipe.OpenListener(dispatcher, Wrap(this, &PipeMsg::OnListenerEvent), Wrap(this, &PipeMsg::OnConnection), pipeName);
	connection = pipe.OpenConnection(dispatcher, Wrap(this, &PipeMsg::OnClientEvent), pipeName);
	pipe.ActivateListener(listener);
	pipe.ActivateConnection(connection);
	pipe.ActivateDispatcher(dispatcher);
	pipe.CloseConnection(connection);
	pipe.CloseListener(listener);
	pipe.CloseDispatcher(dispatcher);

	return true;
}

bool PipeMsg::Initialize() {
	return true;
}

void PipeMsg::Summary() {

}

TObject<IReflect>& PipeMsg::operator () (IReflect& reflect) {
	ReflectClass(PipeMsg);

	return *this;
}

#endif // _WIN32
