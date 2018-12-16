#include "../LeavesWind/LeavesWind.h"
#ifdef _WIN32
#include "../../Driver/Debugger/MiniDump/ZDebuggerWin.h"
// #include <vld.h>
#endif

#include "Loader.h"
#include <ctime>

#if defined(_MSC_VER) && defined(_DEBUG)
#pragma comment(linker, "/STACK:4194304") // typedlua requires more stack memory ...
#endif

using namespace PaintsNow;
using namespace PaintsNow::NsLeavesWind;
using namespace PaintsNow::NsLeavesWing;

bool DumpHandler() {
	// always write minidump file
	return true;
}

int main(int argc, char* argv[]) {
#if defined(_WIN32) || defined(WIN32)
	::CoInitialize(nullptr);
	ZDebuggerWin dumper;
	time_t t;
	time(&t);
	tm* x = localtime(&t);
	char fileName[256];
	sprintf(fileName, "LeavesWingCrashLog_%04d_%02d_%02d_%02d_%02d_%02d.dmp", x->tm_year, x->tm_mon, x->tm_mday, x->tm_hour, x->tm_min, x->tm_sec);
	dumper.SetDumpHandler(fileName, &DumpHandler);
#endif
	IThread* uniqueThreadApiPtr = nullptr;

#if !defined(CMAKE_PAINTSNOW) || ADD_THREAD_PTHREAD
	ZThreadPthread uniqueThreadApi;
	uniqueThreadApiPtr = &uniqueThreadApi;
#endif

	UniqueAllocator allocator(*uniqueThreadApiPtr);
	SetGlobalUniqueAllocator(&allocator);

	CmdLine cmdLine;
	cmdLine.Process(argc, argv);

	Loader loader;
	loader.Load(cmdLine);

#if defined(_WIN32) || defined(WIN32)
	::CoUninitialize();
#endif
	return 0;
}
