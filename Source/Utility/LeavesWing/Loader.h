// Loader.h
// By PaintDream (paintdream@paintdream.com)
// 2016-1-1
//

#ifndef __LOADER_H__
#define __LOADER_H__

#include "Config.h"
#include "CmdLine.h"

#if !defined(CMAKE_PAINTSNOW) || ADD_RENDER_OPENGL
#include "../../Driver/Render/OpenGL/3/ZRenderOpenGL3.h"
#endif

// #if !defined(CMAKE_PAINTSNOW) || ADD_RENDER_DIRECTX9
// #include "../../Driver/Render/DirectX/9/ZRenderDirectX9.h"
// #endif

#if !defined(CMAKE_PAINTSNOW) || ADD_TUNNEL_WINPIPE_BUILTIN
#include "../../Driver/Tunnel/WinPipe/ZWinPipe.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_NETWORK_LIBEVENT
#include "../../Driver/Network/LibEvent/ZNetworkLibEvent.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_ASSET_MODEL_BUILTIN
#include "../../Driver/Asset/Model/MS3D/ZAssetMS3D.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_IMAGE_FREEIMAGE
#include "../../Driver/Image/FreeImage/ZImageFreeImage.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FRAME_FREEGLUT
#include "../../Driver/Timer/Freeglut/ZTimerFreeglut.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FRAME_DXUT
#include "../../Driver/Timer/DXUT/ZTimerDXUT.h"
#endif

#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#if !defined(CMAKE_PAINTSNOW) || ADD_TIMER_TIMERQUEUE_BUILTIN
#include "../../Driver/Timer/WinTimerQueue/ZWinTimerQueue.h"
#endif
#else
#if !defined(CMAKE_PAINTSNOW) || ADD_TIMER_POSIX_BUILTIN
#include "../../Driver/Timer/PosixTimer/ZPosixTimer.h"
#endif
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FRAME_FREEGLUT
#include "../../Driver/Frame/Freeglut/ZFrameFreeglut.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FRAME_DXUT
#include "../../Driver/Frame/DXUT/ZFrameDXUT.h"
#endif


#if !defined(CMAKE_PAINTSNOW) || ADD_ARCHIVE_DIRENT_BUILTIN
#include "../../Driver/Archive/Dirent/ZArchiveDirent.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FILTER_LZMA_BUILTIN
#include "../../Driver/Archive/7Z/ZArchive7Z.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_AUDIO_OPENAL
#include "../../Driver/Audio/OpenAL/ZAudioOpenAL.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_AUDIO_LAME
#include "../../Driver/Audio/Decoder/LAME/ZDecoderLAME.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FONT_FREETYPE
#include "../../Driver/Font/Freetype/ZFontFreetype.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_SCRIPT_LUA53_BUILTIN
#include "../../Driver/Script/Lua/ZScriptLua.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_THREAD_PTHREAD
#include "../../Driver/Thread/Pthread/ZThreadPthread.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_DATABASE_SQLITE3_BUILTIN
#include "../../Driver/Database/Sqlite/ZDatabaseSqlite.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_RANDOM_LIBNOISE_BUILTIN
#include "../../Driver/Random/Libnoise/ZRandomLibnoisePerlin.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_SCRIPT_TCC_BUILTIN
#include "../../Driver/Script/TCC/ZScriptTCC.h"
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FILTER_POD_BUILTIN
#include "../../Driver/Filter/Pod/ZFilterPod.h"
#endif

#include "../../Utility/LeavesWind/LeavesWind.h"


namespace PaintsNow {
	namespace NsLeavesWing {
		class Loader {
		public:
			Loader();
			virtual ~Loader();
			void Load(const CmdLine& cmdLine);
			void LoadOne(const CmdLine::Option& path);
			void SetFactory(const void*& ptr, String& param, const String& key, const std::map<String, CmdLine::Option>& factoryMap);

			const TFactoryBase<IFrame>* frameFactory;
			String paramFrame;
			const TFactoryBase<IRender>* renderFactory;
			String paramRender;
			const TFactoryBase<IThread>* threadFactory;
			String paramThread;
			const TFactoryBase<IAudio>* audioFactory;
			String paramAudio;
			const TFactoryBase<IArchive>* archiveFactory;
			String paramArchive;
			const TFactoryBase<IScript>* scriptFactory;
			String paramScript;
			const TFactoryBase<IScript>* nativeScriptFactory;
			String paramNativeScript;
			const TFactoryBase<ITunnel>* networkFactory;
			String paramNetwork;
			const TFactoryBase<ITunnel>* tunnelFactory;
			String paramTunnel;
			const TFactoryBase<IRandom>* randomFactory;
			String paramRandom;
			const TFactoryBase<ITimer>* timerFactory;
			String paramTimer;
			const TFactoryBase<ITimer>* timerFactoryForFrame;
			String paramTimerFrame;
			const TFactoryBase<IImage>* imageFactory;
			String paramImage;
			const TFactoryBase<IFilterBase>* filterFactory;
			String paramFilter;
			const TFactoryBase<IFontBase>* fontFactory;
			String paramFont;
			const TFactoryBase<IAsset::Model>* modelFactory;
			String paramModel;
			const TFactoryBase<IAudio::Decoder>* decoderFactory;
			String paramDecoder;
			const TFactoryBase<IDatabase>* databaseFactory;
			String paramDatabase;
			const TFactoryBase<IDebugger>* debuggerFactory;
			String paramDebugger;

			template <class T>
			class FactoryOpenGL : public TFactoryBase<IRender> {
			public:
				FactoryOpenGL(Loader* l) : TFactoryBase<IRender>(Wrap(this, &FactoryOpenGL<T>::CreateEx)), loader(l) {}

				IRender* CreateEx(const String& info = "") const {
					assert(loader->thread != nullptr);
					assert(loader->frame != nullptr);
					const ZInt2& size = loader->frame->GetWindowSize();
					VRender* render = new T(*loader->thread, loader->mainThread, (size_t)size.x(), (size_t)size.y());
					return static_cast<IRender*>(render); // just make compiler happy
				}

			private:
				Loader* loader;
			};

			template <class T, class B>
			class FactoryInitWithThread : public TFactoryBase<B> {
			public:
				FactoryInitWithThread(Loader* l) : TFactoryBase<B>(Wrap(this, &FactoryInitWithThread<T, B>::CreateEx)), loader(l) {}

				B* CreateEx(const String& info = "") const {
					return new T(*loader->thread);
				}

			private:
				Loader* loader;
			};


			IThread::Thread* mainThread;
			IThread* thread;
			IFrame* frame;
			NsLeavesWind::LeavesWind* leavesWind;
			Config config;
		};
	}
}


#endif // __LOADER_H__
