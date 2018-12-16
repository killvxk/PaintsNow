// PaintsNow Common Header
// WaitForSingleObject(GetCurrentThread(), INFINITE);
// PaintDream (paintdream@paintdream.com)
// 

// 3rd party libraries:
//		LZMA			OpenGL			GLEW
//		OpenAL			FreeImage		Freeglut
//		Freetype		Lame			Pthread
//		Dirent			Libiconv		LibNoise
//		LibEvent2		Sqlite3			Lua5.4
//		TypedLua		Lpeg			JsonCPP
// Platforms:
//		Linux (i386/amd64/arm) with OpenGL 3.3.3+ supported
//		Windows (i386/amd64) with OpenGL 3.3.3+ supported
// Compilers:
//		GCC (with C++ 11)
//		MSVC6 | MSVC14
//

#ifndef __PAINTSNOW_H__
#define __PAINTSNOW_H__

#if defined(_MSC_VER) && _MSC_VER <= 1200
#define static_assert(f, g) { int check = sizeof(char[2*(int)(f)]); }
#endif

#ifdef CMAKE_PAINTSNOW
#include <Config.h>
#if defined(NDEBUG)
#if defined(_DEBUG)
#undef _DEBUG
#endif
#else
#if !defined(_DEBUG) && defined(_MSC_VER)
#define _DEBUG
#endif
#endif
#else
#define USE_STATIC_THIRDPARTY_LIBRARIES 1
#endif // CMAKE_PAINTSNOW

#ifdef PAINTSNOW_DLL
// From http://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_DLL
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllexport))
#else
#define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllimport))
#else
#define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define DLL_LOCAL
#else
#if __GNUC__ >= 4
#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif
#endif
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif

#ifdef _MSC_VER
#if _MSC_VER <= 1200
#pragma warning(disable:4355)
#pragma warning(disable:4503)
#pragma warning(disable:4666)
#pragma warning(disable:4786)
#pragma comment(linker, "/ignore:4006")
#pragma comment(linker, "/ignore:4098")
#define __STL_HAS_NAMESPACES

// defines C++ keywords for MSVC6
#define override
#define final
#define nullptr (NULL)
#define thread_local __declspec(thread)
namespace std {
	template <class T>
	T& move(T& t) {
		return t;
	}
}
#endif
#pragma warning(disable:4316)
#pragma warning(disable:4819)
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif


#endif // __PAINTSNOW_H__
