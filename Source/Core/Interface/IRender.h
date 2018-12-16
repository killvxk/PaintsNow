// IRender.h - Basic render interface
// By PaintDream (paintdream@paintdream.com)
// 2014-11-30
//

#ifndef __IRENDER_H__
#define __IRENDER_H__

#include "../PaintsNow.h"
#include "IReflect.h"
#include "../Common/ZType.h"
#include "../Template/TProxy.h"
#include "IDevice.h"
#include <string>
#include <map>

namespace PaintsNow {
	class IShader;
	class VRender : public IDevice {
	public:
		virtual ~VRender();

		class Program {}; // Program + VAO
		class Shader {};
		class Notifier {};
		class Buffer {};
		class Target {}; // render buffer
		class Texture {};

		// Some operations must be performed in render thread.
		typedef TWrapper<void, VRender&> ThreadRoutine;
		virtual void QueueRenderThreadRoutine(const ThreadRoutine& routine) = 0;

		// programs
		enum SHADER_TYPE { VERTEX_SHADER = 0, FRAGMENT_SHADER = 1, SHADER_TYPE_COUNT = 2 };
		virtual Program* CreateProgram() = 0;
		virtual void SetShaderSource(Program* program, SHADER_TYPE shaderType, IShader** shaderSource, size_t count, const String& preset) = 0;
		virtual bool CompileProgram(Program* program) = 0;
		virtual void DeleteProgram(Program* program) = 0;

		// buffers
		enum BUFFER_TARGET { VERTEX = 0, VERTEX_INSTANCED, INDEX, UNIFORM, PIXEL_PACK, PIXEL_UNPACK };
		enum PERFORMANCE { STATIC = 0, DYNAMIC, STREAM };
		virtual Buffer* CreateBuffer(BUFFER_TARGET target) = 0;
		template <class T>
		void InitBufferDataEx(Buffer* bufferHandle, const std::vector<T>& t) {
			if (!t.empty()) {
				InitBufferData(bufferHandle, &t[0], sizeof(t[0]) * t.size());
			}
		}

		template <class T>
		void UpdateBufferData(Buffer* bufferHandle, const std::vector<T>& t) {
			if (!t.empty()) {
				void* buffer = MapBufferData(bufferHandle);
				memcpy(buffer, &t[0], sizeof(t[0]) * t.size());
				UnmapBufferData(bufferHandle, buffer);
			}
		}

		void InitBufferDataEx(Buffer* bufferHandle, const String& t) {
			if (!t.empty()) {
				InitBufferData(bufferHandle, const_cast<char*>(t.data()), sizeof(char) * t.size());
			}
		}

		void UpdateBufferData(Buffer* bufferHandle, const String& t) {
			if (!t.empty()) {
				void* buffer = MapBufferData(bufferHandle);
				memcpy(buffer, t.data(), sizeof(char) * t.size());
				UnmapBufferData(bufferHandle, buffer);
			}
		}

		virtual void InitBufferData(Buffer* bufferHandle, const void* data, size_t length, PERFORMANCE perf = PERFORMANCE::STATIC) = 0;
		virtual size_t GetBufferSize(Buffer* bufferHandle) = 0;
		virtual void* MapBufferData(Buffer* bufferHandle) = 0;
		virtual void UnmapBufferData(Buffer* bufferHandle, void* ptr) = 0;
		virtual void DeleteBuffer(Buffer* bufferHandle) = 0;

		// notifiers
		typedef uint32_t NOTIFY_TYPE;
		enum {
			BIND_OBJECT = 1 << 1,
			UNBIND_OBJECT = 1 << 2,
			READ_COMPLETE = 1 << 3,
			WRITE_COMPLETE = 1 << 4,
			ALL_COMPLETE = 1 << 5,
			IS_BUFFER = 1 << 6,
			IS_TEXTURE = 1 << 7,
		};

		virtual Notifier* CreateNotifier(const TWrapper<void, void*, NOTIFY_TYPE>& callback, NOTIFY_TYPE filter) = 0;
		virtual void BindNotifierBuffer(Notifier*, Buffer* bufferHandle) = 0;
		virtual void UnBindNotifierBuffer(Notifier*, Buffer* bufferHandle) = 0;
		virtual void BindNotifierTexture(Notifier*, Texture* textureHandle) = 0;
		virtual void UnBindNotifierTexture(Notifier*, Texture* textureHandle) = 0;
		virtual void DeleteNotifier(Notifier* notifier) = 0;

		// textures
		enum TEXTURE_TARGET { TEXTURE_1D = 0, TEXTURE_2D, TEXTURE_2D_ARRAY, TEXTURE_3D, TEXTURE_TARGET_LIMIT = 0xFFFFFFFF };
		enum DATA_TYPE { FLOAT, FLOAT_HALF, UNSIGNED_BYTE, UNSIGNED_SHORT, UNSIGNED_INT, DATA_TYPE_LIMIT = 0xFFFFFFFF };
		enum COMPRESSION { COMPRESS_RGBA8, COMPRESS_RGBA16F, COMPRESS_RGBA32F, COMPRESSION_LIMIT = 0xFFFFFFFF };
		enum LAYOUT_TYPE { LAYOUT_R, LAYOUT_RG, LAYOUT_RGB, LAYOUT_BGR, LAYOUT_RGBA, LAYOUT_BGRA, LAYOUT_LIMIT = 0xFFFFFFFF };
		virtual Texture* CreateTexture(TEXTURE_TARGET target, size_t layerCount, COMPRESSION compression = COMPRESS_RGBA8, bool repeat = false, bool linear = true) = 0;

		// RGBA data
		virtual void GetTextureSize(Texture* texture, size_t layer, size_t& width, size_t& height) = 0;
		virtual void ReadTexture(Texture* texture, size_t layer, void* data, DATA_TYPE dataType) = 0;
		virtual void WriteTexture(Texture* texture, size_t layer, size_t width, size_t height, const void* data, LAYOUT_TYPE layoutType, DATA_TYPE dataType) = 0;
		virtual void DeleteTexture(Texture* textureHandle) = 0;

		// render target (frame buffer), nullptr for screen
		enum { DEPTH_BUFFER = (size_t)-1 };
		enum CLEAR_MASK { CLEAR_COLOR = 1, CLEAR_DEPTH = 2 };

		virtual Target* CreateTarget(size_t width, size_t height, size_t bufferCount, const VRender::DATA_TYPE* types = nullptr) = 0;
		virtual void GetTargetSize(Target* target, size_t& width, size_t& height) = 0;
		virtual void ClearTarget(Target* target, const ZInt2Pair& rect, int mask = CLEAR_COLOR | CLEAR_DEPTH, const ZFloat4& color = ZFloat4(0, 0, 0, 0), float depth = 1.0f) = 0;
		virtual void DeleteTarget(Target* target) = 0;

		// status settings
		virtual void SetScreenSize(size_t width, size_t height) = 0;
		enum BLEND { ONE, SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA };
		virtual void SetCullFace(Program* program, bool enable) = 0;
		virtual void SetDepthTest(Program* programHandle, bool enable) = 0;
		virtual void SetDepthWrite(Program* programHandle, bool enable) = 0;
		virtual void SetBlend(Program* programHandle, bool enable, BLEND src = SRC_ALPHA, BLEND dst = ONE_MINUS_SRC_ALPHA) = 0;
		enum POLYGON_MODE { FILL, LINE, POINT};
		typedef void* Location;
		virtual void SetPolygonMode(Program* programHandle, POLYGON_MODE mode) = 0;
		virtual Location GetUniformLocation(Program* programHandle, const String& name) = 0;
		virtual Location GetAttribLocation(Program* programHandle, const String& name) = 0;
		virtual Location GetOutputLocation(Program* programHandle, const String& name) = 0;
		virtual Location GetUniformBufferLocation(Program* programHandle, const String& name) = 0;

		// instructions
		virtual void BeginBind(Program* programHandle, VRender::Target* target) = 0;
		virtual void BindUniform(Program* programHandle, Location location, const float* value, size_t unitSize) = 0;
		virtual void BindUniform(Program* programHandle, Location location, const int* value, size_t unitSize) = 0;
		virtual void BindUniform(Program* programHandle, Location location, const ZMatrixFloat4x4& mat) = 0;
		virtual void BindUniform(Program* programHandle, Location location, const ZMatrixFloat3x3& mat) = 0;
		virtual void BindUniform(Program* programHandle, Location location, Texture* textureHandle) = 0;
		virtual void BindUniform(Program* programHandle, Location location, Buffer* bufferHandle, size_t offset = 0, size_t size = 0) = 0;
		virtual void BindAttrib(Program* programHandle, Location location, Buffer* bufferHandle, size_t count) = 0;
		virtual void BindOutput(Program* programHandle, Location index, Texture* texture) = 0;
		virtual void BindIndex(Program* programHandle, Buffer* bufferHandle, size_t from, size_t to = 0, size_t instanceCount = 0) = 0;
		virtual void FireRender(Program* program) = 0;
		virtual void EndBind(Program* programHandle, VRender::Target* target) = 0;
	};
}

#endif // __IRENDER_H__
