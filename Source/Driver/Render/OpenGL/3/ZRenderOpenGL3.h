// ZRenderOpenGL3.h -- OpenGL Render API provider
// By PaintDream (paintdream@paintdream.com)
// 2014-12-1
//

#ifndef __ZRENDEROPENGL3_H__
#define __ZRENDEROPENGL3_H__

#include "../../../../Core/PaintsNow.h"
#include "../../../../Core/Interface/IRender.h"
#include "../../../../Core/Interface/IThread.h"
#include "../../../../Core/Interface/IDeferredObject.h"
#include "../../../../Core/Interface/IReflect.h"
#include <map>
#include <string>
#include <set>
#include <list>

namespace PaintsNow {
	class ZRenderOpenGL3 final : public VRender, public ISyncObject {
	public:
		ZRenderOpenGL3(IThread& threadApi, IThread::Thread* hostThread, size_t width, size_t height);
		virtual ~ZRenderOpenGL3();
		virtual void QueueRenderThreadRoutine(const ThreadRoutine& routine);
		virtual void SetScreenSize(size_t width, size_t height);

		// programs
		virtual Program* CreateProgram();
		virtual void SetShaderSource(Program* program, SHADER_TYPE shaderType, IShader** shaderSource, size_t count, const String& preset);
		virtual bool CompileProgram(Program* program);
		virtual void DeleteProgram(Program* program);

		virtual void SetCullFace(Program* program, bool enable);

		virtual Buffer* CreateBuffer(BUFFER_TARGET target);
		virtual size_t GetBufferSize(Buffer* bufferHandle);
		virtual void InitBufferData(Buffer* bufferHandle, const void* data, size_t length, PERFORMANCE perf = PERFORMANCE::STATIC);
		virtual void* MapBufferData(Buffer* bufferHandle);
		virtual void UnmapBufferData(Buffer* bufferHandle, void* ptr);
		virtual void DeleteBuffer(Buffer* bufferHandle);

		virtual Notifier* CreateNotifier(const TWrapper<void, void*, NOTIFY_TYPE>& callback, NOTIFY_TYPE filter);
		virtual void BindNotifierBuffer(Notifier*, Buffer* bufferHandle);
		virtual void UnBindNotifierBuffer(Notifier*, Buffer* bufferHandle);
		virtual void BindNotifierTexture(Notifier*, Texture* textureHandle);
		virtual void UnBindNotifierTexture(Notifier*, Texture* textureHandle);
		virtual void DeleteNotifier(Notifier* notifier);

		virtual Target* CreateTarget(size_t width, size_t height, size_t bufferCount, const VRender::DATA_TYPE* types);
		virtual void GetTargetSize(Target* target, size_t& width, size_t& height);
		virtual void ClearTarget(Target* target, const ZInt2Pair& rect, int mask = CLEAR_COLOR | CLEAR_DEPTH, const ZFloat4& color = ZFloat4(0, 0, 0, 0), float depth = 1.0f);
		virtual void DeleteTarget(Target* target);
		// textures
		virtual Texture* CreateTexture(TEXTURE_TARGET target, size_t layerCount, COMPRESSION compression = COMPRESS_RGBA8, bool repeat = false, bool linear = true);
		// RGBA data
		virtual void GetTextureSize(Texture* texture, size_t layer, size_t& width, size_t& height);
		virtual void ReadTexture(Texture* texture, size_t layer, void* data, DATA_TYPE dataType);
		virtual void WriteTexture(Texture* texture, size_t layer, size_t width, size_t height, const void* data, LAYOUT_TYPE layout, DATA_TYPE dataType);
		virtual void DeleteTexture(Texture* textureHandle);

		virtual void SetDepthTest(Program* program, bool enable);
		virtual void SetDepthWrite(Program* program, bool enable);
		virtual void SetBlend(Program* program, bool enable, BLEND src, BLEND dst);
		virtual void SetPolygonMode(Program* program, POLYGON_MODE mode);
		// variables binding
		virtual void BeginBind(Program* programHandle, VRender::Target* target);
		virtual Location GetUniformLocation(Program* programHandle, const String& name);
		virtual Location GetAttribLocation(Program* programHandle, const String& name);
		virtual Location GetOutputLocation(Program* programHandle, const String& name);
		virtual Location GetUniformBufferLocation(Program* programHandle, const String& name);
		virtual void BindUniform(Program* programHandle, Location location, const float* value, size_t unitSize);
		virtual void BindUniform(Program* programHandle, Location location, const int* value, size_t unitSize);
		virtual void BindUniform(Program* programHandle, Location location, Texture* textureHandle);
		virtual void BindUniform(Program* programHandle, Location location, Buffer* bufferHandle, size_t offset = 0, size_t size = 0);
		virtual void BindUniform(Program* programHandle, Location location, const ZMatrixFloat4x4& mat);
		virtual void BindUniform(Program* programHandle, Location location, const ZMatrixFloat3x3& mat);
		virtual void BindAttrib(Program* programHandle, Location location, Buffer* bufferHandle, size_t count);
		virtual void BindOutput(Program* target, Location index, Texture* texture);
		virtual void BindIndex(Program* programHandle, Buffer* bufferHandle, size_t from, size_t to = 0, size_t instanceCount = 0);
		virtual void EndBind(Program* programHandle, VRender::Target* target);
		// outputs binding
		// Notice: name is reserved for OpenGL. OpenGL uses slot instead of name to specify an output port.
		// virtual void BindOutput(Program* programHandle, const String& name, Target* target, Texture* textureHandle, size_t slot);

		// render
		virtual void FireRender(Program* program);
		void CheckThread();

		// static functions
		static BLEND ToBlendState(int d);
		static int FromBlendState(BLEND b);
		void FlushDeferredObjects();
		void FlushThreadRoutines();
		size_t GetTextureSize(DATA_TYPE dataType, unsigned int layout, size_t width, size_t height);

		static int GetTextureID(VRender::Texture* texture);
		     
	private:
		IThread::Thread* hostThread;
		size_t screenWidth;
		size_t screenHeight;
		std::set<IDeferredObject*> deferredObjects;
		std::list<ThreadRoutine> queuedThreadRoutines;
	};
}


#endif // __ZRENDEROPENGL3_H__
