#include "LeavesWind.h"
#include <ctime>
#include "../../Driver/Filter/Pod/ZFilterPod.h"
#include "../../Driver/Filter/Json/ZFilterJson.h"
#include "../../Driver/Filter/LZW/ZFilterLZW.h"
#include "../../Driver/Filter/LZMA/ZFilterLZMA.h"

#include "../RayForce/LibLoader.h"

#if defined(__linux__)
#include <dlfcn.h>
#include <signal.h>
typedef bool (*DynamicInit)(PaintsNow::IScript::Request* script, PaintsNow::UniqueAllocator* allocator);
#elif defined(_WIN32)
#include <windows.h>
typedef bool (WINAPI* DynamicInit)(PaintsNow::IScript::Request* script, PaintsNow::UniqueAllocator* allocator);
#endif

using namespace PaintsNow;
using namespace PaintsNow::NsLeavesWind;
using namespace PaintsNow::NsLunaWidget;
using namespace PaintsNow::NsSnowyStream;
using namespace PaintsNow::NsHeartVioliner;
using namespace PaintsNow::NsMythForest;
using namespace PaintsNow::NsFluteLeaves;
using namespace PaintsNow::NsIceSpring;
using namespace PaintsNow::NsBridgeSunset;

LeavesWind::LeavesWind(bool ng, Interfaces& inters, const TWrapper<IArchive*, IStreamBase&, size_t>& subArchiveCreator, size_t threadCount, size_t warpCount) :
					Library(inters.thread),
					interfaces(inters),
					bridgeSunset(inters.thread, inters.script, threadCount),
					flameWork(inters.thread, inters.nativeScript),
					echoLegend(inters.thread, inters.network, bridgeSunset),
					lunaWidget(inters.thread, inters.render, snowyStream),
					snowyStream(inters, subArchiveCreator, Wrap(this, &LeavesWind::OnConsoleOutput)),
					fluteLeaves(inters.thread),
					mythForest(inters, snowyStream, bridgeSunset, fluteLeaves, warpCount),
					heartVioliner(inters.thread, inters.timer, bridgeSunset),
					rayForce(inters.thread, snowyStream),
					remembery(inters.thread, inters.database),
					iceSpring(inters.thread, bridgeSunset),
					galaxyWeaver(inters.thread, inters.tunnel, bridgeSunset, snowyStream, Wrap(this, &LeavesWind::Print)),
					consoleThread(nullptr), resetting(false), nogui(ng)
#if defined(_WIN32) || defined(WIN32)
, comBridge(inters.thread)
#endif
{
#if defined(_WIN32) || defined(WIN32)
	rayForce.Register(&comBridge);
#endif

	// initialize them all
	ModulesControl(true);

	interfaces.frame.SetCallback(this);
	IScript::Request& request = interfaces.script.GetDefaultRequest();
	request.DoLock();
	IScript::Request* initRequest = interfaces.script.NewRequest();
	Require(*initRequest); // register callbacks
	initRequest->ReleaseObject();
	request.UnLock();
}

void LeavesWind::ModulesControl(bool init) {
	IScript::Library* modules[] = {
		&remembery,
		&snowyStream,
		&lunaWidget,
		&mythForest,
		&heartVioliner,
		&fluteLeaves,
		&rayForce,
		&iceSpring,
		&galaxyWeaver
	};

	if (init) {
		for (size_t i = 0; i < sizeof(modules) / sizeof(modules[0]); i++) {
			modules[i]->Initialize();
		}
	} else {
		for (size_t i = sizeof(modules) / sizeof(modules[0]); i > 0; i--) {
			modules[i - 1]->Uninitialize();
		}
	}
}

LeavesWind::~LeavesWind() {
	Reset(false);
	ModulesControl(false);
}

void LeavesWind::EnterStdinLoop() {
	printf("Init Standard Input Environment ...\n");
	ConsoleProc(nullptr, 0);
}

void LeavesWind::EnterMainLoop() {
	BeginConsole();
	interfaces.frame.EnterMainLoop();

#ifdef _WIN32
	::FreeConsole();
	// interfaces.thread.Wait(consoleThread);
#endif
	EndConsole();
}

void LeavesWind::BeginConsole() {
	assert(consoleThread == nullptr);
	consoleThread = interfaces.thread.NewThread(Wrap(this, &LeavesWind::ConsoleProc), 0, false);
}

void LeavesWind::EndConsole() {
	assert(consoleThread != nullptr);
	threadApi.DeleteThread(consoleThread);
	consoleThread = nullptr;
}

#include <iostream>
#ifdef WIN32
#include <Windows.h>

bool ReadConsoleSafe(WCHAR buf[], DWORD size, DWORD* read, HANDLE h) {
	// may crash or memory leak when user close the window on win7
	// but why ?
	__try {
		if (::ReadConsoleW(h, buf, size - 1, read, nullptr)) {
			return true;
		}
	} __except (EXCEPTION_ACCESS_VIOLATION) {
		printf("Unexpected access violation\n");
	}

	return false;
}

#else
#include <sys/poll.h>
#endif

bool LeavesWind::ProcessCommand(const String& command) {
	if (nogui && (command == "quit" || command == "exit")) {
		return false;
	} else {
		// TODO:
		IScript::Request& request = interfaces.script.GetDefaultRequest();
		if (!listenConsole) {
			request.DoLock();
			IScript::Request::Ref code = request.Load(command, "Console");
			if (code) {
				request.Push();
				request.Call(Sync, code);
				request.Pop();
				request.Dereference(code);
			} else {
				request.Error("Invalid input statement");
			}

			request.UnLock();
		} else {
			request.DoLock();
			request.Push();
			request.Call(Sync, listenConsole, command);
			request.Pop();
			request.UnLock();
		}

		return true;
	}
}

bool LeavesWind::ConsoleProc(IThread::Thread* thread, size_t index) {
#ifndef WIN32
	pollfd cinfd[1];
	cinfd[0].fd = fileno(stdin);
	cinfd[0].events = POLLIN;
#else
	HANDLE h = ::GetStdHandle(STD_INPUT_HANDLE);
#endif
	printf("=> ");
	fflush(stdout);
	while (true) {
#ifndef WIN32
		int ret = poll(cinfd, 1, 1000);
		if (ret > 0 && cinfd[0].revents == POLLIN) {
			std::string command;
			getline(std::cin, command);
			if (command[command.size() - 1] == '\n') {
				command = command.substr(0, command.size() - 1);
			}

			// Process Command
			if (!command.empty()) {
				IScript::Request& request = interfaces.script.GetDefaultRequest();
				// remove uncessary spaces
				if (!ProcessCommand(String(command.c_str(), command.length()))) {
					break;
				}
			}
			printf("=> ");
			fflush(stdout);
		} else if (ret < 0){
			break;
		}
#else
		DWORD ret = ::WaitForSingleObject(h, INFINITE);

		if (ret == WAIT_OBJECT_0) {
			const size_t CMD_SIZE = 1024;
			static WCHAR buf[CMD_SIZE];
			DWORD read;
			if (ReadConsoleSafe(buf, CMD_SIZE, &read, h)) {
				String unicode((const char*)buf, sizeof(buf[0]) * read);
				// Process Command
				if (buf[0] != '\0') {
					String command = SystemToUtf8(unicode);
					// remove tail spaces and returns
					size_t end = command.size() - 1;
					while (true) {
						char ch = command[end];
						if (ch == '\r' || ch == '\n' || ch == '\t' || ch == ' ') {
							end--;
						} else {
							break;
						}
					}

					if (!ProcessCommand(command.substr(0, end + 1)))
						break;
				}
				printf("=> ");
			} else {
				break;
			}
		} else if (ret == WAIT_FAILED) {
			break;
		}
#endif
	}

	return false;
}

void LeavesWind::OnInitialize(void* param) {}

template <class T>
void WriteObjectAttrib(IScript::Request& request, T& container) {
	for (typename T::const_iterator p = container.begin(); p != container.end(); ++p) {
		request << key((*p).first) << (*p).second;
	}

	container.clear();
}

template <class T>
void WriteLocalObjectAttrib(IScript::Request& request, IScript::Request::LocalInfo& info, T& container) {
	typename T::iterator p = container.find(info.name);
	if (p != container.end()) {
		request << info << (*p).second;
		container.erase(p);
	}
}

void LeavesWind::RequestSetScreenSize(IScript::Request& request, ZInt2& size) {
	METHOD_ENTRY(LeavesWind::RequestSetScreenSize);
	interfaces.frame.SetWindowSize(size);
}

void LeavesWind::RequestGetScreenSize(IScript::Request& request) {
	request.DoLock();
	request << interfaces.frame.GetWindowSize();
	request.UnLock();
}

void LeavesWind::RequestWarpCursor(IScript::Request& request, ZInt2 position) {
	METHOD_ENTRY(LeavesWind::RequestWarpCursor);
	interfaces.frame.WarpCursor(position);
}

void LeavesWind::RequestShowCursor(IScript::Request& request, String& type) {
	METHOD_ENTRY(LeavesWind::RequestShowCursor);
	IFrame::CURSOR cursor = IFrame::ARROW;
	if (type == "None") {
		cursor = IFrame::NONE;
	} else if (type == "Arrow") {
		cursor = IFrame::ARROW;
	} else if (type == "Cross") {
		cursor = IFrame::CROSS;
	} else if (type == "Wait") {
		cursor = IFrame::WAIT;
	}

	interfaces.frame.ShowCursor(cursor);
}


void LeavesWind::RequestSetAppTitle(IScript::Request& request, String& title) {
	METHOD_ENTRY(LeavesWind::RequestSetAppTitle);
	interfaces.frame.SetWindowTitle(title.c_str());
}

void LeavesWind::RequestForward(IScript::Request& request, IScript::Request::Ref ref) {
	METHOD_ENTRY(LeavesWind::RequestForward);
	CHECK_REFERENCES_WITH_TYPE(ref, IScript::Request::FUNCTION);
	request.DoLock();
	request.Call(Async, ref);
	request.Dereference(ref);
	request.UnLock();
}

bool LeavesWind::IsRendering() const {
	return interfaces.frame.IsRendering();
}

void LeavesWind::Require(IScript::Request& request) {
	// takeover the default print call and redirect it to our console.
	request << global << key("System");
	Library::Require(request);
	request << endtable;

	// disable extra libs
	// request << global << key("package") << nil << endtable;
	request << global << key("io") << nil << endtable;
	// request << global << key("debug") << nil << endtable;
	request << global << key("os") << nil << endtable;

#ifdef _DEBUG
	request << global << key("DEBUG") << true << endtable;
#else
	request << global << key("DEBUG") << false << endtable;
#endif

#ifdef _WIN32
	request << global << key("DLL") << ".dll" << endtable;
#else
	request << global << key("DLL") << ".so" << endtable;
#endif

#ifdef _WIN32
	request << global << key("DLL") << ".dll" << endtable;
#else
	request << global << key("DLL") << ".so" << endtable;
#endif

#if defined(_WIN32)
	char ver[256];
	sprintf(ver, "MSVC %d", _MSC_VER);
	request << global << key("Compiler") << ver << endtable;
#elif defined(__VERSION__)
	request << global << key("Compiler") << __VERSION__ << endtable;
#endif

	request.GetScript()->SetErrorHandler(Wrap(this, &LeavesWind::RequestPrint));
}

void LeavesWind::RequestExit(IScript::Request& request) {
	METHOD_ENTRY(LeavesWind::RequestExit);
	interfaces.frame.ExitMainLoop();

	if (nogui) {
#ifndef WIN32
		raise(SIGINT);
#else
		FreeConsole();
#endif
	}
}

void LeavesWind::RequestListenConsole(IScript::Request& request, IScript::Request::Ref ref) {
	METHOD_ENTRY(LeavesWind::RequestListenConsole);
	if (ref) {
		CHECK_REFERENCES_WITH_TYPE(ref, IScript::Request::FUNCTION);
	}

	if (listenConsole) {
		request.DoLock();
		request.Dereference(listenConsole);
		request.UnLock();
	}

	listenConsole = ref;
}

void LeavesWind::RequestPrint(IScript::Request& request, String& text) {
	METHOD_ENTRY(LeavesWind::RequestPrint);
	Print(text);
}

void LeavesWind::RequestGetFullPath(IScript::Request& request, String& path) {
	METHOD_ENTRY(LeavesWind::RequestGetFullPath);
	String prefix = interfaces.archive.GetMetaData("root");
	request.DoLock();
	request << prefix + path;
	request.UnLock();
}

void LeavesWind::OnConsoleOutput(const String& text) {
	DoLock();
	Print(text);
	UnLock();
}

void LeavesWind::Print(const String& str) {
	// convert utf8 to system encoding
	String text = Utf8ToSystem(str);
#if defined(_WIN32) || defined(WIN32)
	// wprintf(L"%s\n", text.c_str());
	static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD ws;
	WriteConsoleW(handle, text.c_str(), (DWORD)wcslen((const WCHAR*)text.c_str()), &ws, nullptr);
	printf("\n");
#else
	printf("%s\n", text.c_str());
#endif
}

void LeavesWind::ClearRefs() {
#ifdef __linux__
	for (std::list<void*>::iterator t = dynamicRefs.begin(); t != dynamicRefs.end(); ++t) {
		dlclose(*t);
	}

	dynamicRefs.clear();
#endif
}

void LeavesWind::Reset(bool reload) {
	DoLock();

	resetting = true;

	IScript::Request& request = interfaces.script.GetDefaultRequest();
	if (listenConsole) {
		request.DoLock();
		request.Dereference(listenConsole);
		request.UnLock();
	}

	ClearRefs();
	interfaces.script.Reset();
	bridgeSunset.Reset(reload);

	if (reload) {
		IScript::Request& request = interfaces.script.GetDefaultRequest();
		Require(request);
	}

	resetting = false;
	UnLock();
}

void LeavesWind::OnWindowSize(const PaintsNow::ZInt2& size) {
	interfaces.render.SetScreenSize(size.x(), size.y());
	iceSpring.OnResize(interfaces.script.GetDefaultRequest(), size);
}

void LeavesWind::OnMouse(const IFrame::EventMouse& mouse) {
	IFrame::EventMouse reverse = mouse;
	size_t width, height;
	interfaces.render.GetTargetSize(nullptr, width, height);
	reverse.position.y() = (int)height - reverse.position.y();
	iceSpring.OnMouse(interfaces.script.GetDefaultRequest(), reverse);
}

void LeavesWind::OnKeyboard(const IFrame::EventKeyboard& keyboard) {
	iceSpring.OnKeyboard(interfaces.script.GetDefaultRequest(), keyboard);
}

void LeavesWind::OnRender() {
	if (!resetting) {
		DoLock();
		size_t width, height;
		interfaces.render.GetTargetSize(nullptr, width, height);
		interfaces.render.ClearTarget(nullptr, ZInt2Pair(ZInt2(0, 0), ZInt2((int)width, (int)height)));

		lunaWidget.OnRender();
		mythForest.OnRender();

		UnLock();
	}
}

void LeavesWind::RequestLoadLibrary(IScript::Request& request, String& path) {
	METHOD_ENTRY(LeavesWind::RequestLoadLibrary);
	String root = interfaces.archive.GetMetaData("root");
	void* mod = LibLoader::DynamicLoadLibrary(root + path, "Instance", this, &request);
	request.DoLock();
	request << (mod != nullptr);
	request.UnLock();
}

class ExpandParamsScriptTask : public QuickTask {
public:
	ExpandParamsScriptTask(const String& p, const std::vector<String>& params, Interfaces& inters, IceSpring& ice) : path(p), value(params), interfaces(inters), iceSpring(ice) {}

	bool LoadScriptText(const String& path, String& text) {
		size_t length = 0;
		bool ret = false;
		IStreamBase* stream = interfaces.archive.Open(path + "." + interfaces.script.GetFileExt(), false, length);
		if (stream == nullptr) {
			stream = interfaces.archive.Open(path, false, length);
		}

		if (stream != nullptr) {
			if (length != 0) {
				// read string
				text.resize(length);
				if (stream->Read(const_cast<char*>(text.data()), length)) {
					ret = true;
				}
			} else {
				ret = true;
			}

			stream->ReleaseObject();
		}

		return ret;
	}

	virtual void Execute(void* context) override {
		IScript::Request& request = *reinterpret_cast<IScript::Request*>(context);
		String text;
		if (LoadScriptText(path, text)) {
			request.DoLock();
			IScript::Request::Ref ref = request.Load(text, path);
			if (ref) {
				request.Push();
				for (size_t i = 0; i < value.size(); i++) {
					request << value[i];
				}
				request.Call(Sync, ref);
				request.Dereference(ref);
				printf("=> ");
			}

			request.UnLock();
		}

		delete this;
	}

	String path;
	Interfaces& interfaces;
	IceSpring& iceSpring;
	std::vector<String> value;
};

void LeavesWind::Execute(const String& path, const std::vector<String>& params) {
	bridgeSunset.Dispatch(new ExpandParamsScriptTask(String(path), std::vector<String>(params), interfaces, iceSpring));
}

TObject<IReflect>& LeavesWind::operator () (IReflect& reflect) {
	ReflectClass(LeavesWind)[Interface(IScript::Library)];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(snowyStream)[ScriptLibrary = "SnowyStream"];
		ReflectProperty(heartVioliner)[ScriptLibrary = "HeartVioliner"];
		ReflectProperty(lunaWidget)[ScriptLibrary = "LunaWidget"];
		ReflectProperty(mythForest)[ScriptLibrary = "MythForest"];
		ReflectProperty(fluteLeaves)[ScriptLibrary = "FluteLeaves"];
		ReflectProperty(iceSpring)[ScriptLibrary = "IceSpring"];
		ReflectProperty(bridgeSunset)[ScriptLibrary = "BridgeSunset"];
		ReflectProperty(rayForce)[ScriptLibrary = "RayForce"];
		ReflectProperty(echoLegend)[ScriptLibrary = "EchoLegend"];
		ReflectProperty(remembery)[ScriptLibrary = "Remembery"];
		ReflectProperty(galaxyWeaver)[ScriptLibrary = "GalaxyWeaver"];
	}

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestExit)[ScriptMethod = "Exit"];
		ReflectMethod(RequestPrint)[ScriptMethod = "Print"];
		ReflectMethod(RequestForward)[ScriptMethod = "Forward"];
		ReflectMethod(RequestLoadLibrary)[ScriptMethod = "LoadLibrary"];
		ReflectMethod(RequestSetAppTitle)[ScriptMethod = "SetAppTitle"];
		ReflectMethod(RequestShowCursor)[ScriptMethod = "ShowCursor"];
		ReflectMethod(RequestWarpCursor)[ScriptMethod = "WrapCursor"];
		ReflectMethod(RequestGetFullPath)[ScriptMethod = "GetFullPath"];
		ReflectMethod(RequestSetScreenSize)[ScriptMethod = "SetScreenSize"];
		ReflectMethod(RequestGetScreenSize)[ScriptMethod = "GetScreenSize"];
		ReflectMethod(RequestListenConsole)[ScriptMethod = "ListenConsole"];
	}

	return *this;
}


