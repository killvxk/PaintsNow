#include "Loader.h"
#include "../../Utility/RayForce/LibLoader.h"
// #include <vld.h>
#include "../LeavesWind/Platform.h"

using namespace PaintsNow;
using namespace PaintsNow::NsLeavesWind;
using namespace PaintsNow::NsLeavesWing;

#if !defined(CMAKE_PAINTSNOW) || ADD_FILTER_LZMA_BUILTIN
IArchive* Create7ZArchive(IStreamBase& streamBase, size_t length) {
	return new ZArchive7Z(streamBase, length);
}
#endif

class ZFrameDummy : public IFrame {
public:
	ZFrameDummy() {}
	virtual void SetCallback(Callback* callback) {}
	virtual const ZInt2& GetWindowSize() const {
		static ZInt2 size;
		return size;
	}

	virtual void SetWindowSize(const ZInt2& size) {}
	virtual void SetWindowTitle(const String& title) {}

	virtual void OnMouse(const EventMouse& mouse) {}
	virtual void OnKeyboard(const EventKeyboard& keyboard) {}
	virtual void OnRender() {}
	virtual void OnWindowSize(const ZInt2& newSize) {}
	virtual void EnterMainLoop() {}
	virtual void ExitMainLoop() {}
	virtual void ShowCursor(CURSOR cursor) {}
	virtual void WarpCursor(const ZInt2& position) {}
	virtual bool IsRendering() const {
		return true;
	}
};


class ZRenderDummy : public VRender {
public:
	ZRenderDummy(IThread& thread, IThread::Thread* mainThread, size_t width, size_t height);
	virtual void QueueRenderThreadRoutine(const ThreadRoutine& routine);
	virtual void SetScreenSize(size_t width, size_t height);

	// programs
	virtual Program* CreateProgram();
	virtual bool CompileProgram(Program* program);
	virtual void SetShaderSource(Program* program, SHADER_TYPE shaderType, IShader** shaderSource, size_t count, const String& preset);
	virtual void DeleteProgram(Program* program);

	virtual void SetCullFace(Program* program, bool enable);

	virtual Buffer* CreateBuffer(BUFFER_TARGET target);
	virtual size_t GetBufferSize(Buffer* bufferHandle);
	virtual void InitBufferData(Buffer* bufferHandle, const void* data, size_t length, PERFORMANCE perf = PERFORMANCE::STATIC);
	virtual void* MapBufferData(Buffer* bufferHandle);
	virtual void UnmapBufferData(Buffer* bufferHandle, void* ptr);
	virtual void DeleteBuffer(Buffer* bufferHandle);

	virtual Notifier* CreateNotifier(const TWrapper<void, void*, IRender::NOTIFY_TYPE>& callback, NOTIFY_TYPE filter);
	virtual void BindNotifierBuffer(Notifier*, Buffer* bufferHandle);
	virtual void UnBindNotifierBuffer(Notifier*, Buffer* bufferHandle);
	virtual void BindNotifierTexture(Notifier*, Texture* textureHandle);
	virtual void UnBindNotifierTexture(Notifier*, Texture* textureHandle);
	virtual void DeleteNotifier(Notifier* notifier);

	virtual Target* CreateTarget(size_t width, size_t height, size_t bufferCount, const IRender::DATA_TYPE* types);
	virtual void GetTargetSize(Target* target, size_t& width, size_t& height);
	virtual void CopyTarget(Target* to, const ZInt2Pair& toRect, size_t toIndex, Target* from, const ZInt2Pair& fromRect, size_t fromIndex);
	virtual void ReadTarget(Target* target, size_t index, const ZInt2Pair& rect, IRender::Buffer* buffer);
	virtual void ClearTarget(Target* target, const ZInt2Pair& rect, int m, const ZFloat4& color, float depthask);
	virtual void DeleteTarget(Target* target);
	// textures
	virtual Texture* CreateTexture(TEXTURE_TARGET target, size_t layerCount, COMPRESSION compression, bool repeat, bool linear);
	// RGBA data
	virtual void GetTextureSize(Texture* texture, size_t layer, size_t& width, size_t& height);
	virtual void ReadTexture(Texture* texture, size_t layer, void* data, DATA_TYPE dataType);
	virtual void WriteTexture(Texture* texture, size_t layer, size_t width, size_t height, const void* data, LAYOUT_TYPE layout, DATA_TYPE dataType);
	virtual void DeleteTexture(Texture* textureHandle);

	virtual void SetDepthTest(Program* programHandle, bool enable);
	virtual void SetDepthWrite(Program* programHandle, bool enable);
	virtual void SetBlend(Program* programHandle, bool enable, BLEND src, BLEND dst);

	virtual void SetPolygonMode(Program* programHandle, POLYGON_MODE mode);
	// variables binding
	virtual void BeginBind(Program* programHandle, IRender::Target* target);
	virtual Location GetUniformLocation(Program* programHandle, const String& name);
	virtual Location GetAttribLocation(Program* programHandle, const String& name);
	virtual Location GetOutputLocation(Program* programHandle, const String& name);
	virtual Location GetUniformBufferLocation(Program* programHandle, const String& name);
	virtual void BindUniform(Program* programHandle, Location location, const float* value, size_t unitSize);
	virtual void BindUniform(Program* programHandle, Location location, const int* value, size_t unitSize);
	virtual void BindUniform(Program* programHandle, Location location, Texture* textureHandle);
	virtual void BindUniform(Program* programHandle, Location location, Buffer* bufferHandle, size_t offset, size_t size);
	virtual void BindUniform(Program* programHandle, Location location, const ZMatrixFloat4x4& mat);
	virtual void BindUniform(Program* programHandle, Location location, const ZMatrixFloat3x3& mat);
	virtual void BindAttrib(Program* programHandle, Location location, Buffer* bufferHandle, size_t count);
	virtual void BindOutput(Program* target, Location index, Texture* texture);
	virtual void BindIndex(Program* programHandle, Buffer* bufferHandle, size_t from, size_t to, size_t instanceCount);
	virtual void EndBind(Program* programHandle, IRender::Target* target);
	virtual void FireRender(Program* program);
};

Loader::Loader() : frameFactory(nullptr), renderFactory(nullptr), threadFactory(nullptr), audioFactory(nullptr), archiveFactory(nullptr), scriptFactory(nullptr), networkFactory(nullptr), timerFactory(nullptr), imageFactory(nullptr), filterFactory(nullptr), fontFactory(nullptr), modelFactory(nullptr), decoderFactory(nullptr), databaseFactory(nullptr) {}

Loader::~Loader() {}

void Loader::LoadOne(const CmdLine::Option& option) {
	LibLoader::DynamicLoadLibrary(option.name, "Config", static_cast<LeavesApi*>(&config));
}

void Loader::SetFactory(const void*& ptr, String& param, const String& key, const std::map<String, CmdLine::Option>& factoryMap) {
	const std::map<String, CmdLine::Option>::const_iterator p = factoryMap.find(key);
	if (p != factoryMap.end()) {
		const String& impl = p->second.name;
		const std::list<Config::Entry>& entries = config.GetEntry(key);
		for (std::list<Config::Entry>::const_iterator q = entries.begin(); q != entries.end(); ++q) {
			// printf("Scanning [%s] = %s (%s)\n", key.c_str(), (*q).name.c_str(), param.c_str());
			if ((ptr == (*q).factoryBase && impl.empty()) || (*q).name == impl) {
				ptr = (*q).factoryBase;
				param = p->second.param;
				// printf("Factory[%s] = %s (%s)\n", key.c_str(), impl.c_str(), param.c_str());
				break;
			}
		}
	}

	if (ptr == nullptr) {
		fprintf(stderr, "Couldn't find factory for %s\n", key.c_str());
		exit(-1);
	}
}


void Loader::Load(const CmdLine& cmdLine) {

	// Load necessary modules
	const std::list<CmdLine::Option>& modules = cmdLine.GetModuleList();

	printf("LeavesWing %s\nPaintDream (paintdream@paintdream.com) (C) 2014-2018\nBased on PaintsNow [https://github.com/paintdream/paintsnow]\n", PAINTSNOW_VERSION_MINOR);

	bool nogui = true;
	bool enableModuleLog = false;
	for (std::list<CmdLine::Option>::const_iterator it = modules.begin(); it != modules.end(); ++it) {
		LoadOne(*it);
	}

	const std::map<String, CmdLine::Option>& configMap = cmdLine.GetConfigMap();
	std::string entry = "Entry";

	for (std::map<String, CmdLine::Option>::const_iterator p = configMap.begin(); p != configMap.end(); ++p) {
		if ((*p).first == "Graphic") {
			nogui = (*p).second.name != "true";
		} else if ((*p).first == "Log") {
			enableModuleLog = (*p).second.name == "true";
		} else if ((*p).first == "Entry") {
			entry = (*p).second.name;
		}
	}

	const std::map<String, CmdLine::Option>& factoryMap = cmdLine.GetFactoryMap();

	if (enableModuleLog) {
		for (std::map<String, CmdLine::Option>::const_iterator i = factoryMap.begin(); i != factoryMap.end(); ++i) {
			printf("Load config [%s] = %s Param: %s\n", (*i).first.c_str(), (*i).second.name.c_str(), (*i).second.param.c_str());
			assert(factoryMap.find(i->first) != factoryMap.end());
			// printf("Compare result: %d\n", strcmp(i->first.c_str(), "IRender"));
		}
	}

	// assert(factoryMap.find(String("IArchive")) != factoryMap.end());
	// assert(factoryMap.find(String("IRender")) != factoryMap.end());

	// Load default settings

	const TFactory<ZFrameDummy, IFrame> sdummyframe;
	frameFactory = &sdummyframe;
	config.RegisterFactory("IFrame", "ZFrameDummy", sdummyframe);

#if !defined(CMAKE_PAINTSNOW) || ADD_FRAME_FREEGLUT
	const TFactory<ZFrameFreeglut, IFrame> sframeFactory;
	if (!nogui) {
		frameFactory = &sframeFactory;
	}

	config.RegisterFactory("IFrame", "ZFrameFreeglut", sframeFactory);

	const TFactory<ZTimerFreeglut, ITimer> stimerFactoryDef;
	config.RegisterFactory("ITimer", "ZTimerFreeglut", stimerFactoryDef);
	config.RegisterFactory("ITimerForFrame", "ZTimerFreeglut", stimerFactoryDef);
	timerFactoryForFrame = &stimerFactoryDef;
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FRAME_DXUT
#ifdef CMAKE_PAINTSNOW
	const TFactory<ZFrameDXUT, IFrame> sframeFactoryDXUT;
	const TFactory<ZTimerDXUT, ITimer> stimerFactoryDXUT;
	config.RegisterFactory("IFrame", "ZFrameDXUT", sframeFactoryDXUT);
	config.RegisterFactory("ITimerForFrame", "ZTimerDXUT", stimerFactoryDXUT);
	config.RegisterFactory("ITimer", "ZTimerDXUT", stimerFactoryDXUT);
#endif
#endif
	const FactoryOpenGL<ZRenderDummy> srenderFactoryDummy(this);
	renderFactory = &srenderFactoryDummy;
	config.RegisterFactory("IRender", "ZRenderDummy", srenderFactoryDummy);

#if !defined(CMAKE_PAINTSNOW) || ADD_RENDER_OPENGL
	const FactoryOpenGL<ZRenderOpenGL3> srenderFactory(this);
	if (!nogui) {
		renderFactory = &srenderFactory;
		config.RegisterFactory("IRender", "ZRenderOpenGL3", srenderFactory);
	}
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_RENDER_DIRECTX
#ifdef CMAKE_PAINTSNOW
	if (!nogui) {
		const FactoryOpenGL<ZRenderDirectX9> srenderFactoryDX9(this);
		config.RegisterFactory("IRender", "ZRenderDirectX9", srenderFactoryDX9);
	}
#endif
#endif

	TWrapper<IArchive*, IStreamBase&, size_t> subArchiveCreator;

#if !defined(CMAKE_PAINTSNOW) || ADD_FILTER_LZMA_BUILTIN
	subArchiveCreator = Create7ZArchive;
#endif

	if (!nogui) {
		// Must have render factory in GUI mode.
		assert(renderFactory != nullptr);
	}

#if !defined(CMAKE_PAINTSNOW) || ADD_SCRIPT_LUA53_BUILTIN
	const FactoryInitWithThread<ZScriptLua, IScript> sscriptFactory(this);
	scriptFactory = &sscriptFactory;
	config.RegisterFactory("IScript", "ZScriptLua", sscriptFactory);
#endif

	// Must have script factory.
	assert(scriptFactory != nullptr);

#if !defined(CMAKE_PAINTSNOW) || ADD_SCRIPT_TCC_BUILTIN
	const FactoryInitWithThread<ZScriptTCC, IScript> sscriptTccFactory(this);
	nativeScriptFactory = &sscriptTccFactory;
	config.RegisterFactory("INativeScript", "ZScriptTCC", sscriptTccFactory);
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_NETWORK_LIBEVENT
	const FactoryInitWithThread<ZNetworkLibEvent, ITunnel> snetworkFactory(this);

	networkFactory = &snetworkFactory;
	config.RegisterFactory("INetwork", "ZNetworkLibEvent", *networkFactory);
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_TUNNEL_WINPIPE_BUILTIN
	const TFactory<ZWinPipe, ITunnel> stunnelFactory;

	tunnelFactory = &stunnelFactory;
	config.RegisterFactory("ITunnel", "ZWinPipe", *tunnelFactory);
#else
	tunnelFactory = networkFactory;
#endif

	// Must have network factory.
	assert(networkFactory != nullptr);
	assert(tunnelFactory != nullptr);

#if !defined(CMAKE_PAINTSNOW) || ADD_IMAGE_FREEIMAGE
	static const TFactory<ZImageFreeImage, IImage> simageFactory;
	imageFactory = &simageFactory;
	config.RegisterFactory("IImage", "ZImageFreeImage", simageFactory);
#endif

	// Must have image factory.
	assert(imageFactory != nullptr);

#if !defined(CMAKE_PAINTSNOW) || ADD_RANDOM_LIBNOISE_BUILTIN
	static const TFactory<ZRandomLibnoisePerlin, IRandom> srandomFactory;
	randomFactory = &srandomFactory;
	config.RegisterFactory("IRandom", "ZRandomLibnoisePerlin", srandomFactory);
#endif

	// Must have random factory.
	assert(randomFactory != nullptr);

#if !defined(CMAKE_PAINTSNOW) || ADD_FONT_FREETYPE
	static const TFactory<ZFontFreetype, IFontBase> sfontFactory;
	fontFactory = &sfontFactory;
	config.RegisterFactory("IFontBase", "ZFontFreetype", sfontFactory);
#endif

	if (!nogui) {
		assert(fontFactory != nullptr);
	}

#if !defined(CMAKE_PAINTSNOW) || ADD_ASSET_MODEL_BUILTIN
	static const TFactory<ZAssetMS3D, IAsset::Model> smodelFactory;
	modelFactory = &smodelFactory;
	config.RegisterFactory("IAsset::Model", "ZAssetMS3D", smodelFactory);
#endif


#if !defined(CMAKE_PAINTSNOW) || ADD_AUDIO_LAME
	static const TFactory<ZDecoderLAME, IAudio::Decoder> sdecoderFactory;
	decoderFactory = &sdecoderFactory;
	config.RegisterFactory("IAudio::Decoder", "ZDecoderLAME", sdecoderFactory);
#endif

	assert(decoderFactory != nullptr);

#if !defined(CMAKE_PAINTSNOW) || ADD_DATABASE_SQLITE3_BUILTIN
	static const TFactory<ZDatabaseSqlite, IDatabase> sdatabaseFactory;
	databaseFactory = &sdatabaseFactory;
	config.RegisterFactory("IDatabase", "ZDatabaseSqlite", sdatabaseFactory);
#endif

	assert(databaseFactory != nullptr);

#if (defined(_WIN32) || defined(WIN32)) && (!defined(CMAKE_PAINTSNOW) || ADD_TIMER_TIMERQUEUE_BUILTIN)
	static const TFactory<ZWinTimerQueue, ITimer> stimerFactory;
	config.RegisterFactory("ITimer", "ZWinTimerQueue", stimerFactory);
	// if (timerFactory == nullptr)
	timerFactory = &stimerFactory;

	if (timerFactoryForFrame == nullptr) {
		timerFactoryForFrame = timerFactory;
	}
#endif

#if !(defined(_WIN32) || defined(WIN32)) && (!defined(CMAKE_PAINTSNOW) || ADD_TIMER_POSIX_BUILTIN)
	static const TFactory<ZPosixTimer, ITimer> pstimerFactory;
	config.RegisterFactory("ITimer", "ZPosixTimer", pstimerFactory);
	if (timerFactory == nullptr) {
		timerFactory = &pstimerFactory;
	}

	if (timerFactoryForFrame == nullptr) {
		timerFactoryForFrame = &pstimerFactory;
	}
#endif

	assert(timerFactory != nullptr);

#if !defined(CMAKE_PAINTSNOW) || ADD_THREAD_PTHREAD
	static const TFactory<ZThreadPthread, IThread> sthreadFactory;
	threadFactory = &sthreadFactory;
	config.RegisterFactory("IThread", "ZThreadPthread", sthreadFactory);
#endif

	assert(threadFactory != nullptr);

#if !defined(CMAKE_PAINTSNOW) || ADD_AUDIO_OPENAL
	static const TFactory<ZAudioOpenAL, IAudio> saudioFactory;
	audioFactory = &saudioFactory;
	config.RegisterFactory("IAudio", "ZAudioOpenAL", saudioFactory);
#endif

	assert(audioFactory != nullptr);

#if !defined(CMAKE_PAINTSNOW) || ADD_ARCHIVE_DIRENT_BUILTIN
	static const TFactoryConstruct<ZArchiveDirent, IArchive> sarchiveFactory;

	archiveFactory = &sarchiveFactory;
	config.RegisterFactory("IArchive", "ZArchiveDirent", sarchiveFactory);
#endif

#if !defined(CMAKE_PAINTSNOW) || ADD_FILTER_POD_BUILTIN
	static const TFactory<ZFilterPod, IFilterBase> sfilterFactory;
	filterFactory = &sfilterFactory;
	config.RegisterFactory("IFilterBase", "ZFilterPod", sfilterFactory);
#else
	static const TFactory<NoFilter, IFilterBase> sfilterFactory;
	filterFactory = &sfilterFactory;
#endif

	assert(archiveFactory != nullptr);

	SetFactory(reinterpret_cast<const void*&>(renderFactory), paramRender, "IRender", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(frameFactory), paramFrame, "IFrame", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(threadFactory), paramThread, "IThread", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(audioFactory), paramAudio, "IAudio", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(archiveFactory), paramArchive, "IArchive", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(scriptFactory), paramScript, "IScript", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(nativeScriptFactory), paramNativeScript, "INativeScript", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(networkFactory), paramNetwork, "INetwork", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(randomFactory), paramRandom, "IRandom", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(timerFactory), paramTimer, "ITimer", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(timerFactoryForFrame), paramTimerFrame, "ITimerForFrame", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(imageFactory), paramImage, "IImage", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(filterFactory), paramFont, "IFilterBase", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(fontFactory), paramFont, "IFontBase", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(modelFactory), paramModel, "IAsset::Model", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(decoderFactory), paramDecoder, "IAudio::Decoder", factoryMap);
	SetFactory(reinterpret_cast<const void*&>(databaseFactory), paramDatabase, "IDatabase", factoryMap);

	thread = (*threadFactory)(paramThread);
	mainThread = thread->OpenCurrentThread();
	{
		IScript* script = (*scriptFactory)(paramScript);
		IScript* nativeScript = (*nativeScriptFactory)(paramNativeScript);
		frame = (*frameFactory)(paramFrame);
		IRender* render = (*renderFactory)(paramRender);
		ITimer* timer = (*timerFactory)(paramTimer);
		IImage* image = (*imageFactory)(paramImage);
		INetwork* network = static_cast<INetwork*>((*networkFactory)(paramNetwork));
		ITunnel* tunnel = (*tunnelFactory)(paramTunnel);
		IAudio* audio = (*audioFactory)(paramAudio);
		IArchive* archive = (*archiveFactory)(paramArchive);
		IRandom* random = (*randomFactory)(paramRandom);
		IDatabase* database = (*databaseFactory)(paramDatabase);
		IFilterBase* filter = (*filterFactory)(paramFilter);
		IFontBase* font = (*fontFactory)(paramFont);

		{
			Interfaces interfaces(*archive, *audio, *database, *filter, *font, *frame, *image, *network, *random, *render, *script, *nativeScript, *thread, *timer, *tunnel);
			LeavesWind leavesWind(nogui, interfaces, subArchiveCreator, 4, 0);
			config.PostRuntimeState(&leavesWind, LeavesApi::RUNTIME_INITIALIZE);
			std::vector<String> paramList = Split(paramScript);
			leavesWind.Execute(entry, paramList);

			std::map<String, CmdLine::Option>::const_iterator quit = configMap.find("Quit");
			if (quit == configMap.end() || (*quit).second.name != "true") {
				if (nogui) {
					leavesWind.EnterStdinLoop();
				} else {
					leavesWind.EnterMainLoop();
				}
			}

			// leavesWind.Reset(false);
			config.PostRuntimeState(&leavesWind, LeavesApi::RUNTIME_UNINITIALIZE);
		}

		font->ReleaseDevice();
		filter->ReleaseDevice();
		database->ReleaseDevice();
		random->ReleaseDevice();
		tunnel->ReleaseDevice();
		image->ReleaseDevice();
		network->ReleaseDevice();
		nativeScript->ReleaseDevice();
		script->ReleaseDevice();
		timer->ReleaseDevice();
		render->ReleaseDevice();
		frame->ReleaseDevice();
		audio->ReleaseDevice();
		archive->ReleaseDevice();
	}

	thread->DeleteThread(mainThread);
	delete thread;
}


// Dummy render

ZRenderDummy::ZRenderDummy(IThread& thread, IThread::Thread* mainThread, size_t width, size_t height) {}
void ZRenderDummy::SetScreenSize(size_t width, size_t height) {}

void ZRenderDummy::QueueRenderThreadRoutine(const ThreadRoutine& routine) {

}

// programs
IRender::Program* ZRenderDummy::CreateProgram() {
	return nullptr;
}

void ZRenderDummy::DeleteProgram(Program* program) {}

bool ZRenderDummy::CompileProgram(Program* program) {
	return true;
}

void ZRenderDummy::SetShaderSource(Program* program, SHADER_TYPE shaderType, IShader** shaderSource, size_t count, const String& preset) {}

void ZRenderDummy::SetCullFace(Program* program, bool enable) {

}

IRender::Buffer* ZRenderDummy::CreateBuffer(BUFFER_TARGET target) {
	return nullptr;
}

void ZRenderDummy::InitBufferData(Buffer* bufferHandle, const void* data, size_t length, PERFORMANCE perf) {}

void* ZRenderDummy::MapBufferData(Buffer* bufferHandle) {
	return nullptr;
}

void ZRenderDummy::UnmapBufferData(Buffer* bufferHandle, void* ptr) {}

void ZRenderDummy::DeleteBuffer(Buffer* bufferHandle) {}

IRender::Target* ZRenderDummy::CreateTarget(size_t width, size_t height, size_t bufferCount, const IRender::DATA_TYPE* dataTypes) {
	return nullptr;
}

void ZRenderDummy::GetTargetSize(Target* target, size_t& width, size_t& height) {}

void ZRenderDummy::CopyTarget(Target* to, const ZInt2Pair& toRect, size_t toIndex, Target* from, const ZInt2Pair& fromRect, size_t fromIndex) {}

void ZRenderDummy::ReadTarget(Target* target, size_t index, const ZInt2Pair& rect, IRender::Buffer* buffer) {}

void ZRenderDummy::ClearTarget(Target* target, const ZInt2Pair& rect, const int m, const ZFloat4& color, float depthask) {}

void ZRenderDummy::DeleteTarget(Target* target) {}

// textures
IRender::Texture* ZRenderDummy::CreateTexture(TEXTURE_TARGET target, size_t layerCount, COMPRESSION compression, bool repeat, bool linear) {
	return nullptr;
}

// RGBA data
void ZRenderDummy::WriteTexture(Texture* texture, size_t layer, size_t width, size_t height, const void* data, LAYOUT_TYPE layoutType, DATA_TYPE dataType) {}

void ZRenderDummy::ReadTexture(Texture* texture, size_t layer, void* data, DATA_TYPE dataType) {}

void ZRenderDummy::DeleteTexture(Texture* textureHandle) {}

void ZRenderDummy::GetTextureSize(Texture* texture, size_t layer, size_t& width, size_t& height) {}

void ZRenderDummy::SetDepthTest(Program* program, bool enable) {}
void ZRenderDummy::SetDepthWrite(Program* program, bool enable) {}


void ZRenderDummy::SetBlend(Program* program, bool enable, BLEND src, BLEND dst) {}

// variables binding
void ZRenderDummy::BeginBind(Program* programHandle, IRender::Target* target) {}

IRender::Location ZRenderDummy::GetUniformLocation(Program* programHandle, const String& name) {
	return 0;
}

IRender::Location ZRenderDummy::GetOutputLocation(Program* programHandle, const String& name) {
	return 0;
}

IRender::Location ZRenderDummy::GetAttribLocation(Program* programHandle, const String& name) {
	return 0;
}

IRender::Location ZRenderDummy::GetUniformBufferLocation(Program* programHandle, const String& name) {
	return 0;
}

void ZRenderDummy::BindUniform(Program* programHandle, Location location, const float* value, size_t unitSize) {}

void ZRenderDummy::BindUniform(Program* programHandle, Location location, const int* value, size_t unitSize) {}

void ZRenderDummy::BindUniform(Program* programHandle, Location location, Texture* textureHandle) {}

void ZRenderDummy::BindUniform(Program* programHandle, Location location, Buffer* bufferHandle, size_t offset, size_t size) {}

void ZRenderDummy::BindUniform(Program* programHandle, Location location, const ZMatrixFloat4x4& mat) {}

void ZRenderDummy::BindUniform(Program* programHandle, Location location, const ZMatrixFloat3x3& mat) {}

void ZRenderDummy::BindAttrib(Program* programHandle, Location location, Buffer* bufferHandle, size_t count) {}

void ZRenderDummy::BindIndex(Program* programHandle, Buffer* bufferHandle, size_t from, size_t to, size_t instanceCount) {}

void ZRenderDummy::EndBind(Program* programHandle, IRender::Target* target) {}

void ZRenderDummy::FireRender(Program* program) {}

size_t ZRenderDummy::GetBufferSize(Buffer* bufferHandle) {
	return 0;
}

void ZRenderDummy::SetPolygonMode(Program* program, POLYGON_MODE mode) {}

void ZRenderDummy::BindOutput(Program* target, Location index, Texture* texture) {}

IRender::Notifier* ZRenderDummy::CreateNotifier(const TWrapper<void, void*, IRender::NOTIFY_TYPE>& callback, NOTIFY_TYPE filter) {
	static IRender::Notifier dummyNotifier;
	return &dummyNotifier;
}

void ZRenderDummy::BindNotifierBuffer(Notifier*, Buffer* bufferHandle) {}
void ZRenderDummy::UnBindNotifierBuffer(Notifier*, Buffer* bufferHandle) {}
void ZRenderDummy::BindNotifierTexture(Notifier*, Texture* textureHandle) {}
void ZRenderDummy::UnBindNotifierTexture(Notifier*, Texture* textureHandle) {}
void ZRenderDummy::DeleteNotifier(Notifier* notifier) {}