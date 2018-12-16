// LostDream Test Entry
#if defined(_WIN32) || defined(WIN32)
#include <comdef.h>
#endif
#include "../LeavesWind/LeavesWind.h"

#if !defined(CMAKE_PAINTSNOW) || ADD_THREAD_PTHREAD
#include "../../Driver/Thread/Pthread/ZThreadPthread.h"
#endif

#include "LostDream.h"
#include "Spatial/Spatial.h"
#include "Reflection/Reflection.h"
#include "../LeavesWind/Platform.h"

using namespace PaintsNow;
using namespace PaintsNow::NsLostDream;

#if defined(_MSC_VER) && _MSC_VER <= 1200 && _DEBUG
extern "C" int _CrtDbgReport() {
	return 0;
}
#endif

// #include <vld.h>
// int testex(void);

int main(void) {
#if defined(_WIN32) || defined(WIN32)
	::CoInitialize(nullptr);
#endif
	IThread* uniqueThreadApiPtr = nullptr;

#if !defined(CMAKE_PAINTSNOW) || ADD_THREAD_PTHREAD
	ZThreadPthread uniqueThreadApi;
	uniqueThreadApiPtr = &uniqueThreadApi;
#endif

	UniqueAllocator allocator(*uniqueThreadApiPtr);
	SetGlobalUniqueAllocator(&allocator);
	// return testex();

	LostDream lostDream;
	lostDream.RegisterQualifier(TFactory<RPC, LostDream::Qualifier>(), 1);
	/*
	lostDream.RegisterQualifier(TFactory<Serialization, LostDream::Qualifier>(), 1);
	#ifdef _WIN32
	lostDream.RegisterQualifier(TFactory<PipeMsg, LostDream::Qualifier>(), 1);
	#endif
	lostDream.RegisterQualifier(TFactory<Annotation, LostDream::Qualifier>(), 1);
	lostDream.RegisterQualifier(TFactory<RandomQuery, LostDream::Qualifier>(), 12);*/
	
	lostDream.RunQualifiers(true, 0, 4);

#if defined(_WIN32) || defined(WIN32)
	::CoUninitialize();
#endif
	return 0;
}

