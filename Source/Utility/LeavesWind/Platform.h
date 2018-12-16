// Platform.h
// PaintDream (paintdream@paintdream.com)
// 2018-8-24
//

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "../../Core/PaintsNow.h"
#define PAINTSNOW_VERSION_MAJOR "0"
#define PAINTSNOW_VERSION_MINOR "0.18.12.16 [Dev Build]"

#ifdef _MSC_VER
#if !defined(CMAKE_PAINTSNOW) || ADD_AUDIO_LAME
#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "libmp3lameD.lib")
#pragma comment(lib, "mpghipD.lib")
#else
#pragma comment(lib, "libmp3lame.lib")
#pragma comment(lib, "mpghip.lib")
#endif
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_RENDER_OPENGL
#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "glew32sd.lib")
#else
#pragma comment(lib, "glew32s.lib")
#endif
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_AUDIO_OPENAL
#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "OpenAL32D.lib")
#else
#pragma comment(lib, "OpenAL32.lib")
#endif
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FONT_FREETYPE
#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "freetypeD.lib")
#else
#pragma comment(lib, "freetype.lib")
#endif
#endif

#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "iconv32D.lib")
#else
#pragma comment(lib, "iconv32.lib")
#endif


#if !defined(CMAKE_PAINTSNOW) || ADD_IMAGE_FREEIMAGE
#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "freeimageD.lib")
#else
#pragma comment(lib, "freeimage.lib")
#endif
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FRAME_FREEGLUT
#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "freeglut_staticD.lib")
#else
#pragma comment(lib, "freeglut_static.lib")
#endif
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FRAME_DXUT_BUILTIN
#if defined(CMAKE_PAINTSNOW)
#define USE_DIRECTX9
#if USE_STATIC_THIRDPARTY_LIBRARIES
#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "d3dx9d.lib")
#pragma comment(lib, "d3dx10d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#endif
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "dxguid.lib")
#endif
#endif
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_THREAD_PTHREAD
#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "pthreadD.lib")
#else
#pragma comment(lib, "pthread.lib")
#endif
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_NETWORK_LIBEVENT
#if defined(_DEBUG) && _MSC_VER > 1200
#pragma comment(lib, "libeventD.lib")
#else
#pragma comment(lib, "libevent.lib")
#endif
#endif

#endif


#if defined(_MSC_VER) && !defined(CMAKE_PAINTSNOW)
#pragma comment(lib, "comctl32.lib")
#if _MSC_VER > 1200
#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/PaintsNow.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/HeartVioliner.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/FlameWork.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/MythForest.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/SnowyStream.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/LunaWidget.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/BridgeSunset.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/FluteLeaves.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/IceSpring.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/LeavesWind.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/RayForce.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/EchoLegend.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/Remembery.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Debug/GalaxyWeaver.lib")
#else
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/PaintsNow.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/HeartVioliner.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/FlameWork.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/MythForest.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/SnowyStream.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/LunaWidget.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/BridgeSunset.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/FluteLeaves.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/IceSpring.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/LeavesWind.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/RayForce.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/EchoLegend.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/Remembery.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Debug/GalaxyWeaver.lib")
#endif
#else
#ifdef _WIN64
#if defined(_MT) && defined(_DLL)
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/PaintsNow.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/HeartVioliner.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/FlameWork.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/SnowyStream.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/MythForest.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/LunaWidget.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/BridgeSunset.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/FluteLeaves.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/IceSpring.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/LeavesWind.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/RayForce.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/EchoLegend.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/Remembery.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/ReleaseDynamicCRT/GalaxyWeaver.lib")
#else
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/PaintsNow.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/HeartVioliner.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/FlameWork.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/SnowyStream.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/MythForest.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/LunaWidget.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/BridgeSunset.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/FluteLeaves.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/IceSpring.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/LeavesWind.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/RayForce.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/EchoLegend.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/Remembery.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/x64/Release/GalaxyWeaver.lib")
#endif
#else
#if defined(_MT) && defined(_DLL)
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/PaintsNow.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/HeartVioliner.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/FlameWork.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/SnowyStream.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/MythForest.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/LunaWidget.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/BridgeSunset.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/FluteLeaves.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/IceSpring.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/LeavesWind.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/RayForce.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/EchoLegend.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/Remembery.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/ReleaseDynamicCRT/GalaxyWeaver.lib")
#else
#pragma comment(lib, "../../../../Build/Windows/VS/Release/PaintsNow.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/HeartVioliner.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/FlameWork.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/SnowyStream.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/MythForest.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/LunaWidget.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/BridgeSunset.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/FluteLeaves.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/IceSpring.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/LeavesWind.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/RayForce.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/EchoLegend.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/Remembery.lib")
#pragma comment(lib, "../../../../Build/Windows/VS/Release/GalaxyWeaver.lib")
#endif
#endif
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../../../../Build/Windows/VC/PaintsNow/Debug/PaintsNow.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/HeartVioliner/Debug/HeartVioliner.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/FlameWork/Debug/FlameWork.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/SnowyStream/Debug/SnowyStream.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/MythForest/Debug/MythForest.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/LunaWidget/Debug/LunaWidget.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/BridgeSunset/Debug/BridgeSunset.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/FluteLeaves/Debug/FluteLeaves.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/IceSpring/Debug/IceSpring.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/LeavesWind/Debug/LeavesWind.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/RayForce/Debug/RayForce.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/EchoLegend/Debug/EchoLegend.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/Remembery/Debug/Remembery.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/GalaxyWeaver/Debug/GalaxyWeaver.lib")
#else
#pragma comment(lib, "../../../../Build/Windows/VC/PaintsNow/Release/PaintsNow.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/HeartVioliner/Release/HeartVioliner.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/FlameWork/Release/FlameWork.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/SnowyStream/Release/SnowyStream.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/MythForest/Release/MythForest.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/LunaWidget/Release/LunaWidget.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/BridgeSunset/Release/BridgeSunset.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/FluteLeaves/Release/FluteLeaves.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/IceSpring/Release/IceSpring.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/LeavesWind/Release/LeavesWind.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/RayForce/Release/RayForce.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/EchoLegend/Release/EchoLegend.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/Remembery/Release/Remembery.lib")
#pragma comment(lib, "../../../../Build/Windows/VC/GalaxyWeaver/Release/GalaxyWeaver.lib")
#endif
#endif
#endif // _MSC_VER

#endif