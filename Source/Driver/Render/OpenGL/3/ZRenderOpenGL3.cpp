#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "../../../../Core/Interface/Interfaces.h"
#include "ZRenderOpenGL3.h"
#include "../../../../Core/Common/ZShaderWalker.h"
#include "../../../../Core/Interface/IDeferredObject.h"
#include <cstdio>
#include <vector>
#include <iterator>
#include <sstream>

#if USE_STATIC_THIRDPARTY_LIBRARIES
#define GLEW_STATIC
#endif

#include <GL/glew.h>
using namespace PaintsNow;


class GLErrorGuard {
public:
	GLErrorGuard() {
		Guard();
	}

	void Guard() {
#ifdef _DEBUG
		int err = glGetError();
		// fprintf(stderr, "GL ERROR: %d\n", err);
		if (err != 0) {
			int test = 0;
		}
		assert(err == 0);
#endif
	}

	~GLErrorGuard() {
		Guard();
	}
};

struct NotifierImpl3;
struct RenderResource : public IDeferredObject {
	RenderResource(VRender::NOTIFY_TYPE initType) : state(initType), notifier(nullptr) {}

	virtual ~RenderResource();
	virtual void Modify() {
		state &= ~VRender::WRITE_COMPLETE;
		IDeferredObject::Modify();
	}

	VRender::NOTIFY_TYPE state;
	NotifierImpl3* notifier;
};

struct NotifierImpl3 : public VRender::Notifier {
	VRender::NOTIFY_TYPE notifyFilter;
	TWrapper<void, void*, VRender::NOTIFY_TYPE> callback;
	std::set<RenderResource*> objects;

	NotifierImpl3(const TWrapper<void, void*, VRender::NOTIFY_TYPE>& cb, VRender::NOTIFY_TYPE t) : callback(cb), notifyFilter(t) {

	}

	~NotifierImpl3() {
		std::set<RenderResource*> temp;
		std::swap(temp, objects);
		for (std::set<RenderResource*>::iterator p = temp.begin(); p != temp.end(); ++p) {
			if (notifyFilter & VRender::UNBIND_OBJECT) {
				callback(*p, VRender::UNBIND_OBJECT);
			}

			(*p)->notifier = nullptr;
		}
	}

	void BindObject(RenderResource* resource) {
		assert(objects.count(resource) == 0);
		resource->notifier = this;
		objects.insert(resource);

		if (notifyFilter & VRender::BIND_OBJECT) {
			callback(resource, VRender::BIND_OBJECT);
		}
	}

	void UnBindObject(RenderResource* resource) {
		assert(objects.count(resource) != 0);
		resource->notifier = nullptr;
		objects.erase(resource);

		if (notifyFilter & VRender::UNBIND_OBJECT) {
			callback(resource, VRender::UNBIND_OBJECT);
		}
	}

	void Notify(RenderResource* object, VRender::NOTIFY_TYPE filter) {
		object->state |= filter;
		if (callback) {
			if ((object->state & notifyFilter) == notifyFilter)
				callback(object, filter);

			// check all type notification
			bool complete = true;
			for (std::set<RenderResource*>::iterator p = objects.begin(); p != objects.end(); ++p) {
				if (!((*p)->state & filter)) {
					complete = false;
					break;
				}
			}

			if (complete) {
				filter |= VRender::ALL_COMPLETE;
				if ((filter & notifyFilter) == notifyFilter) {
					callback(nullptr, filter);
				}
			}
		}
	}
};

RenderResource::~RenderResource() {
	if (notifier != nullptr) {
		notifier->UnBindObject(this);
	}
}

class BufferImpl3 : public VRender::Buffer, public RenderResource {
public:
	BufferImpl3(unsigned int type) : bufferID(0), bufferType(type), length(0), instanced(false), RenderResource(VRender::IS_BUFFER) {

	}

	virtual ~BufferImpl3() {
	}

	virtual void Synchronize() {
		if (isReleased) {
			glDeleteBuffers(1, &bufferID);
			delete this;
		} else if (bufferID == 0) {
			GLErrorGuard guard;
			glGenBuffers(1, &bufferID);
			assert(bufferID != 0);
			// initialize data if needed
			if (!bufferData.empty()) {
				length = bufferData.size();
				glBindBuffer(bufferType, bufferID);
				glBufferData(bufferType, length, &bufferData[0], perf);
				glBindBuffer(bufferType, 0);
				if (notifier != nullptr)
					notifier->Notify(this, VRender::WRITE_COMPLETE);
			}
		} else if (!bufferData.empty()) {
			GLErrorGuard guard;
			glBindBuffer(bufferType, bufferID);

			// reinit
			if (length != bufferData.size()) {
				length = bufferData.size();
				glBufferData(bufferType, length, &bufferData[0], perf);
			} else {
				glBufferSubData(bufferType, 0, length, &bufferData[0]);
				/*
				void* target = glMapBuffer(bufferType, GL_WRITE_ONLY);
				memcpy(target, &bufferData[0], length);
				glUnmapBuffer(bufferType);*/
			}

			glBindBuffer(bufferType, 0);
			// bufferData.clear();
			if (notifier != nullptr)
				notifier->Notify(this, VRender::WRITE_COMPLETE);
		}
	}

	unsigned int bufferID;
	unsigned int bufferType;
	unsigned int perf;
	std::string bufferData;
	size_t length;
	bool instanced;
};


class TextureImpl3 : public VRender::Texture, public RenderResource {
public:
	TextureImpl3() : textureID(0), RenderResource(VRender::IS_TEXTURE) {}
	virtual void Synchronize() {
		if (isReleased) {
			glDeleteTextures(1, &textureID);
			delete this;
			return;
		} else if (textureID == 0) {
			GLErrorGuard guard;
			glGenTextures(1, &textureID);
			glBindTexture(textureType, textureID);
			if (repeat) {
				glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
			} else {
				glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			if (linear) {
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			} else {
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}

		}

		for (size_t i = 0; i < layers.size(); i++) {
			Layer& layer = layers[i];
			GLsizei width = (GLsizei)layer.width;
			GLsizei height = (GLsizei)layer.height;
			String& bufferData = layer.bufferData;
			unsigned int type = layer.type;
			unsigned int layout = layer.layout;
			const void* data = !bufferData.empty() ? &bufferData[0] : nullptr;

			if (width != 0 && height != 0 && layer.dataChanged) {
				GLErrorGuard guard;
				glBindTexture(textureType, textureID);

				switch (textureType) {
					case GL_TEXTURE_1D:
					{
						glTexImage1D(textureType, 0, compression, width, 0, layout, type, data);
					}
					break;
					case GL_TEXTURE_2D:
					{
						glTexImage2D(textureType, 0, compression, width, height, 0, layout, type, data);
					}
					break;
					case GL_TEXTURE_2D_ARRAY:
					case GL_TEXTURE_3D:
					{
						glTexImage3D(textureType, 0, compression, width, height, 1, 0, layout, type, data);
					}
					break;
				}
				/*
				if (textureType == GL_TEXTURE_1D) {
					glTexSubImage1D(textureType, 0, 0, width, layout == VRender::RGBA ? GL_RGBA : GL_BGRA, type, !bufferData.empty() ? &bufferData[0] : nullptr);
				} else {
					glTexSubImage2D(textureType, 0, 0, 0, width, height, layout == VRender::RGBA ? GL_RGBA : GL_BGRA, type, !bufferData.empty() ? &bufferData[0] : nullptr);
				}*/

				layer.dataChanged = false;
				layer.bufferData.resize(0);

				if (notifier != nullptr)
					notifier->Notify(this, VRender::WRITE_COMPLETE);
			}
		}
	}

	unsigned int textureID;
	unsigned int textureType;
	unsigned int compression;
	bool linear;
	bool repeat;

	struct Layer {
		Layer() : width(0), height(0), type(GL_UNSIGNED_BYTE), dataChanged(false), layout(GL_RGBA) {}
		size_t width, height;
		unsigned int type; // input data type
		bool dataChanged;
		unsigned int layout;
		String bufferData;
	};

	std::vector<Layer> layers;
};

class TargetImpl3 : public VRender::Target, public IDeferredObject {
public:
	TargetImpl3() : frameBuffer(0) {}
	virtual void Synchronize() {
		GLErrorGuard guard;
		if (isReleased) {
			glDeleteFramebuffers(1, &frameBuffer);
			glDeleteRenderbuffers(1, &depthBuffer);
			glDeleteRenderbuffers((GLsizei)renderBuffers.size(), &renderBuffers[0]);
			delete this;
		} else if (frameBuffer == 0) {
			// int err0 = glGetError();
			// assert(err0 == 0);
			glGenFramebuffers(1, &frameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, (GLsizei)width, (GLsizei)height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
			// int err1 = glGetError();
			// assert(err1 == 0);

			size_t bufferCount = renderBuffers.size();
			glGenRenderbuffers((GLsizei)bufferCount, &renderBuffers[0]);
			// int err15 = glGetError();
			// assert(err15 == 0);
			for (size_t i = 0; i < bufferCount; i++) {
				glBindRenderbuffer(GL_RENDERBUFFER, renderBuffers[i]);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, (GLsizei)width, (GLsizei)height);
				// size_t n = renderBuffers[i];
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLsizei)i, GL_RENDERBUFFER, renderBuffers[i]);
				// int err2 = glGetError();
				// assert(err2 == 0);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// int err3 = glGetError();
			// assert(err3 == 0);
		}
	}

	unsigned int frameBuffer;
	unsigned int depthBuffer; // -1
	std::vector<unsigned int> renderBuffers;
	std::vector<unsigned int> renderBufferTypes;
	size_t width, height;

	struct ClearInfo {
		ZInt2Pair rect;
		ZFloat4 color;
		float depth;
		int mask;
	} clearInfo;
};

class ProgramImpl3 : public VRender::Program, public IDeferredObject {
public:
	virtual void Synchronize() {
		if (isReleased) {
			glDeleteProgram(programID);
			for (size_t n = 0; n < sizeof(shaders) / sizeof(shaders[0]); n++) {
				glDeleteShader(shaders[n].shaderID);
			}
			glDeleteVertexArrays(1, &vertexArrayBuffer);
			delete this;
		}
	}

	unsigned int programID;
	struct Shader {
		Shader() : shaderID(0) {}
		unsigned int shaderID;
		std::vector<IShader*> shaderPtrs;	
		String config;
	} shaders[VRender::SHADER_TYPE_COUNT];

	unsigned int vertexArrayBuffer;
	BufferImpl3* indexBuffer;
	TargetImpl3* cacheTarget;
//	unsigned int indexBuffer;
	size_t from, to, instanceCount;
	bool outputBindStateChanged;
	bool enableDepthWrite;
	bool enableDepthTest;
	bool enableBlend;
	bool enableCullFace;
	unsigned int blendSrc;
	unsigned int blendDst;
	unsigned int polygonMode;

	std::vector<TextureImpl3*> textures;
	std::vector<TextureImpl3*> texturesForBuffer;
	std::vector<BufferImpl3*> buffers;
	std::map<VRender::Location, int> mapLocationToTextures;
	std::map<VRender::Location, int> mapLocationToBuffers;
};


class GlewInit {
public:
	GlewInit() {
		glewExperimental = true;
		glewInit();
		printf("%s\n%s\n", glGetString(GL_VERSION), glGetString(GL_VENDOR));
	}
	~GlewInit() {
	}
};

ZRenderOpenGL3::ZRenderOpenGL3(IThread& api, IThread::Thread* h, size_t width, size_t height) : ISyncObject(api), hostThread(h) {
	static GlewInit init;
	// GLErrorGuard guard;
	SetScreenSize(width, height);
}

void ZRenderOpenGL3::FlushDeferredObjects() {
	FlushThreadRoutines();

	if (!deferredObjects.empty()) {
		DoLock();
		for (std::set<IDeferredObject*>::iterator p = deferredObjects.begin(); p != deferredObjects.end(); ++p) {
			IDeferredObject* object = *p;
			object->Update();
		}

		deferredObjects.clear();
		UnLock();
	}
}

ZRenderOpenGL3::~ZRenderOpenGL3() {
	FlushDeferredObjects();
}

void ZRenderOpenGL3::QueueRenderThreadRoutine(const ThreadRoutine& routine) {
	queuedThreadRoutines.push_back(routine);
}

void ZRenderOpenGL3::FlushThreadRoutines() {
	if (!queuedThreadRoutines.empty()) {
		std::list<ThreadRoutine> routines;
		std::swap(routines, queuedThreadRoutines);
		for (std::list<ThreadRoutine>::const_iterator it = routines.begin(); it != routines.end(); ++it) {
			(*it)(*this);
		}
	}
}

void ZRenderOpenGL3::CheckThread() {
	FlushDeferredObjects();
#ifdef _DEBUG
	IThread::Thread* current = threadApi.OpenCurrentThread();
	assert(threadApi.EqualThread(current, hostThread));
	threadApi.DeleteThread(current);
#endif // _DEBUG
}

static void DoClear(const ZInt2Pair& rect, int m, const ZFloat4& color, float depth) {
	glViewport(rect.first.x(), rect.first.y(), rect.second.x() - rect.first.x(), rect.second.y() - rect.first.y());
	glClearColor(color.r(), color.g(), color.b(), color.a());
	glClearDepth(depth);

	int mask = 0;
	mask |= (m & VRender::CLEAR_DEPTH) ? GL_DEPTH_BUFFER_BIT : 0;
	mask |= (m & VRender::CLEAR_COLOR) ? GL_COLOR_BUFFER_BIT : 0;
	glClear(mask);
}

void ZRenderOpenGL3::BeginBind(VRender::Program* p, VRender::Target* tar) {
	CheckThread();

	ProgramImpl3* program = static_cast<ProgramImpl3*>(p);
	TargetImpl3* target = static_cast<TargetImpl3*>(tar);

	GLErrorGuard guard;
	glUseProgram(program->programID);
	glBindVertexArray(program->vertexArrayBuffer);
	glPolygonMode(GL_FRONT_AND_BACK, program->polygonMode);

	if (program->enableBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(program->blendSrc, program->blendDst);
	} else {
		glDisable(GL_BLEND);
	}

	if (program->enableCullFace) {
		glEnable(GL_CULL_FACE);
	} else {
		glEnable(GL_CULL_FACE);
	}

	if (program->enableDepthTest) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}

	glDepthMask(program->enableDepthWrite);

	// Resolve render target change
	if (target != nullptr) {
		if (program->outputBindStateChanged || program->cacheTarget != target) {
			glBindFramebuffer(GL_FRAMEBUFFER, target->frameBuffer);
			assert(program->texturesForBuffer.size() == target->renderBuffers.size());
			for (size_t i = 0; i < target->renderBuffers.size(); i++) {
				TextureImpl3* texture = program->texturesForBuffer[i];
				GLuint slot = GL_COLOR_ATTACHMENT0 + (GLsizei)i;
				if (texture == nullptr) {
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, slot, GL_RENDERBUFFER, target->renderBuffers[i]);
				} else {
					if (texture->textureType == GL_TEXTURE_2D) {
						glFramebufferTexture2D(GL_FRAMEBUFFER, slot, GL_TEXTURE_2D, texture->textureID, 0);
					} else {
						glFramebufferTexture1D(GL_FRAMEBUFFER, slot, GL_TEXTURE_1D, texture->textureID, 0);
					}
				}
			}

			size_t slotCount = target->renderBuffers.size();
			const size_t MAX_ID = 8;
			static GLuint idlist[MAX_ID] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0 + 1,
				GL_COLOR_ATTACHMENT0 + 2, GL_COLOR_ATTACHMENT0 + 3, GL_COLOR_ATTACHMENT0 + 4,
				GL_COLOR_ATTACHMENT0 + 5, GL_COLOR_ATTACHMENT0 + 6, GL_COLOR_ATTACHMENT0 + 7 };
			glDrawBuffers((GLsizei)Min(MAX_ID, slotCount), idlist);

			program->outputBindStateChanged = false;
		}

		program->cacheTarget = target;
		// check completeness
#ifdef _DEBUG
		unsigned int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		assert(status == GL_FRAMEBUFFER_COMPLETE);
#endif

		if (target->clearInfo.mask != 0) {
			DoClear(target->clearInfo.rect, target->clearInfo.mask, target->clearInfo.color, target->clearInfo.depth);
			target->clearInfo.mask = 0;
			if (!(target->clearInfo.rect == ZInt2Pair(ZInt2(0, 0), ZInt2((int)target->width, (int)target->height)))) {
				glViewport(0, 0, (GLsizei)target->width, (GLsizei)target->height);
			}
		}
	} else {
		if (program->cacheTarget != target) {
			glBindFramebuffer(GL_FRAMEBUFFER, target->frameBuffer);
			glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);
		}
	}
}

int ZRenderOpenGL3::GetTextureID(VRender::Texture* texture) {
	TextureImpl3* t = static_cast<TextureImpl3*>(texture);
	return t->textureID;
}

void ZRenderOpenGL3::EndBind(VRender::Program* programHandle, VRender::Target* target) {
	CheckThread();
	// glBindVertexArray(0);
	// glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glBindVertexArray(0);
	glFlush();
}

void ZRenderOpenGL3::BindAttrib(VRender::Program* program, Location location, VRender::Buffer* buffer, size_t count) {
	CheckThread();
	// ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	BufferImpl3* bufferHandle = static_cast<BufferImpl3*>(buffer);

	GLErrorGuard guard;
	assert((size_t)location != (size_t)-1);
	assert(bufferHandle != nullptr);
	glEnableVertexAttribArray((int)(size_t)location);
	assert(bufferHandle->bufferType == GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, bufferHandle->bufferID);
	glVertexAttribPointer((int)(size_t)location, (GLsizei)count, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor((int)(size_t)location, bufferHandle->instanced ? 1 : 0);
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ZRenderOpenGL3::BindIndex(Program* program, Buffer* buffer, size_t from, size_t to, size_t instanceCount) {
	CheckThread();
	GLErrorGuard guard;
	ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	BufferImpl3* bufferHandle = static_cast<BufferImpl3*>(buffer);
	assert(bufferHandle != nullptr);
	assert(bufferHandle->bufferType == GL_ELEMENT_ARRAY_BUFFER);

	programHandle->indexBuffer = bufferHandle;
	programHandle->from = from;
	programHandle->to = to;
	programHandle->instanceCount = instanceCount;
}

VRender::Texture* ZRenderOpenGL3::CreateTexture(TEXTURE_TARGET target, size_t layerCount, COMPRESSION compression, bool repeat, bool linear) {
	// GLErrorGuard guard;
	assert(layerCount != 0);
	TextureImpl3* t = new TextureImpl3();
	t->textureID = 0;
	t->notifier = nullptr;
	switch (target) {
		case TEXTURE_1D:
			t->textureType = GL_TEXTURE_1D;
			break;
		case TEXTURE_2D:
			t->textureType = GL_TEXTURE_2D;
			break;
		case TEXTURE_2D_ARRAY:
			t->textureType = GL_TEXTURE_2D_ARRAY;
			break;
		case TEXTURE_3D:
			t->textureType = GL_TEXTURE_3D;
			break;
		default:
			assert(false);
			break;
	}

	switch (compression) {
		case COMPRESS_RGBA8:
			t->compression = GL_RGBA;
			break;
		case COMPRESS_RGBA16F:
			t->compression = GL_RGBA16F;
			break;
		case COMPRESS_RGBA32F:
			t->compression = GL_RGBA32F;
			break;
		default:
			assert(false);
			break;
	}



	t->linear = linear;
	t->repeat = repeat;
	t->layers.resize(layerCount);
	t->Modify();

	DoLock();
	deferredObjects.insert(t);
	UnLock();
	return t;
}

// std::set<VRender::Texture*> deleted;
void ZRenderOpenGL3::DeleteTexture(VRender::Texture* texture) {
	/*
	if (deleted.count(texture)) {
		int a = 0;
	}
	deleted.insert(texture);*/

//	GLErrorGuard guard;
	TextureImpl3* textureHandle = static_cast<TextureImpl3*>(texture);

	textureHandle->ReleaseObject();
	DoLock();
	deferredObjects.insert(textureHandle);
	UnLock();
}

static unsigned int ConvertBufferType(VRender::BUFFER_TARGET target) {
	switch (target) {
	case VRender::BUFFER_TARGET::VERTEX:
	case VRender::BUFFER_TARGET::VERTEX_INSTANCED:
		return GL_ARRAY_BUFFER;
	case VRender::BUFFER_TARGET::INDEX:
		return GL_ELEMENT_ARRAY_BUFFER;
	case VRender::BUFFER_TARGET::UNIFORM:
		return GL_UNIFORM_BUFFER;
	case VRender::BUFFER_TARGET::PIXEL_PACK:
		return GL_PIXEL_PACK_BUFFER;
	case VRender::BUFFER_TARGET::PIXEL_UNPACK:
		return GL_PIXEL_UNPACK_BUFFER;
	}

	assert(false);
	return GL_ARRAY_BUFFER;
}



VRender::Buffer* ZRenderOpenGL3::CreateBuffer(BUFFER_TARGET target) {
//	GLErrorGuard guard;
	BufferImpl3* buf = new BufferImpl3(ConvertBufferType(target));
	buf->instanced = target == VERTEX_INSTANCED;
	buf->notifier = nullptr;
	buf->perf = GL_STATIC_DRAW;
	buf->Modify();

	DoLock();
	deferredObjects.insert(buf);
	UnLock();

	return buf;
}

void ZRenderOpenGL3::DeleteBuffer(VRender::Buffer* buffer) {
//	GLErrorGuard guard;
	BufferImpl3* bufferHandle = static_cast<BufferImpl3*>(buffer);

	bufferHandle->ReleaseObject();
	DoLock();
	deferredObjects.insert(bufferHandle);

	// printf("COUNT = %d", ++widgetCount);

	UnLock();
}

static unsigned int ConvertPerformance(VRender::PERFORMANCE perf) {
	switch (perf) {
	case VRender::PERFORMANCE::DYNAMIC:
		return GL_DYNAMIC_DRAW;
	case VRender::PERFORMANCE::STATIC:
		return GL_STATIC_DRAW;
	case VRender::PERFORMANCE::STREAM:
		return GL_STREAM_DRAW;
	}

	assert(false);
	return GL_DYNAMIC_DRAW;
}

void ZRenderOpenGL3::InitBufferData(Buffer* buffer, const void* data, size_t length, PERFORMANCE perf) {
//	GLErrorGuard guard;
	assert(length != 0);

	DoLock();
	BufferImpl3* bufferHandle = static_cast<BufferImpl3*>(buffer);
	bufferHandle->length = 0; // force flush
	bufferHandle->perf = ConvertPerformance(perf);
	bufferHandle->bufferData.resize(length);
	if (data != nullptr) {
		memcpy(&bufferHandle->bufferData[0], data, length);
	}

	bufferHandle->Modify();

	deferredObjects.insert(bufferHandle);
	UnLock();
}

size_t ZRenderOpenGL3::GetBufferSize(VRender::Buffer* buffer) {
	BufferImpl3* bufferHandle = static_cast<BufferImpl3*>(buffer);

	return bufferHandle->length;
}

void* ZRenderOpenGL3::MapBufferData(VRender::Buffer* buffer) {
	GLErrorGuard guard;
	DoLock();
	BufferImpl3* bufferHandle = static_cast<BufferImpl3*>(buffer);
	assert(bufferHandle->length != 0);
	assert(!bufferHandle->bufferData.empty());
	// bufferHandle->bufferData.resize(bufferHandle->length);

	UnLock();

	return &bufferHandle->bufferData[0];

/*
	glBindBuffer(bufferHandle->bufferType, bufferHandle->bufferID);
	void* data = nullptr;
	if (!read && write) {
		glBufferData(bufferHandle->bufferType, bufferHandle->length, nullptr, GL_DYNAMIC_DRAW);
		data = glMapBuffer(bufferHandle->bufferType, GL_WRITE_ONLY);
	} else {
		if (!read) {
			data = glMapBuffer(bufferHandle->bufferType, GL_WRITE_ONLY);
		} else {
			data = glMapBuffer(bufferHandle->bufferType, GL_READ_WRITE);
		}
	}
	glBindBuffer(bufferHandle->bufferType, 0);*/
	// return data;
}

void ZRenderOpenGL3::UnmapBufferData(VRender::Buffer* buffer, void* ptr) {
	GLErrorGuard guard;

	DoLock();
	BufferImpl3* bufferHandle = static_cast<BufferImpl3*>(buffer);
	/*
	glBindBuffer(bufferHandle->bufferType, bufferHandle->bufferID);
	glUnmapBuffer(bufferHandle->bufferType);
	glBindBuffer(bufferHandle->bufferType, 0);*/
	assert(!bufferHandle->bufferData.empty());
	bufferHandle->Modify();

	deferredObjects.insert(bufferHandle);
	UnLock();
}
/*
static unsigned int ConvertDataFormat(VRender::DATA_FORMAT dataFormat) {
	switch (dataFormat) {
	case VRender::DATA_FORMAT::RGBA:
		return GL_RGBA;
	case VRender::DATA_FORMAT::DEPTH:
		return GL_DEPTH_COMPONENT;
	}

	assert(false);
	return GL_RGBA;
}*/


static unsigned int ConvertDataType(VRender::DATA_TYPE dataType) {
	switch (dataType) {
	case VRender::DATA_TYPE::FLOAT:
		return GL_FLOAT;
	case VRender::DATA_TYPE::FLOAT_HALF:
		return GL_FLOAT; // also mapped to float
	case VRender::DATA_TYPE::UNSIGNED_BYTE:
		return GL_UNSIGNED_BYTE;
	case VRender::DATA_TYPE::UNSIGNED_SHORT:
		return GL_UNSIGNED_SHORT;
	case VRender::DATA_TYPE::UNSIGNED_INT:
		return GL_UNSIGNED_INT;
	}

	assert(false);

	return GL_FLOAT;
}

size_t ZRenderOpenGL3::GetTextureSize(DATA_TYPE dataType, unsigned int layout, size_t width, size_t height) {
	size_t s = sizeof(float);
	switch (dataType) {
	case FLOAT:
		s = sizeof(float);
		break;
	case UNSIGNED_BYTE:
		s = sizeof(unsigned char);
		break;
	case UNSIGNED_SHORT:
		s = sizeof(unsigned short);
		break;
	case UNSIGNED_INT:
		s = sizeof(unsigned int);
		break;
	}

	// TODO: update it if new types were added.
	size_t count = 0;
	switch (layout) {
		case GL_R:
			count = 1;
			break;
		case GL_RG:
			count = 2;
			break;
		case GL_RGB:
		case GL_BGR:
			count = 3;
			break;
		case GL_RGBA:
		case GL_BGRA:
			count = 4;
			break;
		default:
			assert(false);
			break;
	}
	
	return width * height * count * s;
}

void ZRenderOpenGL3::GetTextureSize(VRender::Texture* tex, size_t layerIndex, size_t& width, size_t& height) {
	TextureImpl3* texture = static_cast<TextureImpl3*>(tex);
	assert(layerIndex < texture->layers.size());
	TextureImpl3::Layer& layer = texture->layers[layerIndex];
	width = layer.width;
	height = layer.height;
}

void ZRenderOpenGL3::ReadTexture(VRender::Texture* tex, size_t layerIndex, void* data, DATA_TYPE dataType) {
	CheckThread();
	GLErrorGuard guard;
	TextureImpl3* texture = static_cast<TextureImpl3*>(tex);
	assert(layerIndex < texture->layers.size());
	TextureImpl3::Layer& layer = texture->layers[layerIndex];

	if (layer.bufferData.size() != 0) {
		memcpy(data, layer.bufferData.data(), layer.bufferData.size());
	} else {
		unsigned int type = ConvertDataType(dataType);
		glBindTexture(texture->textureType, texture->textureID);
		glGetTexImage(texture->textureType, (int)layerIndex, layer.layout, type, data);
		glBindTexture(texture->textureType, 0);
	}
}

void ZRenderOpenGL3::WriteTexture(VRender::Texture* tex, size_t layerIndex, size_t width, size_t height, const void* data, LAYOUT_TYPE layout, DATA_TYPE dataType) {
	// CheckThread();
	DoLock();
	TextureImpl3* texture = static_cast<TextureImpl3*>(tex);
	texture->state &= ~VRender::WRITE_COMPLETE;
	assert(layerIndex < texture->layers.size());
	TextureImpl3::Layer& layer = texture->layers[layerIndex];
	layer.width = width;
	layer.height = height;

	switch (layout) {
		case LAYOUT_R:
			layer.layout = GL_R;
			break;
		case LAYOUT_RG:
			layer.layout = GL_RG;
			break;
		case LAYOUT_RGB:
			layer.layout = GL_RGB;
			break;
		case LAYOUT_BGR:
			layer.layout = GL_BGR;
			break;
		case LAYOUT_BGRA:
			layer.layout = GL_BGRA;
			break;
		case LAYOUT_RGBA:
			layer.layout = GL_RGBA;
			break;
		default:
			assert(false);
			break;
	}
	unsigned int type = ConvertDataType(dataType);
	layer.type = type;
	layer.dataChanged = true;

	if (data != nullptr) {
		size_t length = GetTextureSize(dataType, layer.layout, layer.width, layer.height);
		layer.bufferData.resize(length);
		memcpy(&layer.bufferData[0], data, length);
	}

	texture->Modify();
	deferredObjects.insert(texture);
	UnLock();

/*
	GLErrorGuard guard;
	unsigned int type = ConvertDataType(dataType);
	glBindTexture(texture->textureType, texture->textureID);
	if (texture->textureType == GL_TEXTURE_1D) {
		glTexSubImage1D(texture->textureType, 0, 0, texture->width, layout == RGBA ? GL_RGBA : GL_BGRA, type, data);
	} else {
		glTexSubImage2D(texture->textureType, 0, 0, 0, texture->width, texture->height, layout == RGBA ? GL_RGBA : GL_BGRA, type, data);
	}

	glBindTexture(texture->textureType, 0);*/
}

VRender::Target* ZRenderOpenGL3::CreateTarget(size_t width, size_t height, size_t bufferCount, const VRender::DATA_TYPE* types) {
	assert(width < 0xFFFF && height < 0xFFFF);
	DoLock();
	TargetImpl3* target = new TargetImpl3();
	target->width = width;
	target->height = height;
	target->renderBuffers.resize(bufferCount, 0);
	if (types != nullptr) {
		for (size_t i = 0; i < bufferCount; i++) {
			VRender::DATA_TYPE type = types[i];
			int converted;
			switch (type) {
			case VRender::FLOAT:
				converted = GL_RGBA32F;
				break;
			case VRender::FLOAT_HALF:
				converted = GL_RGBA16F;
				break;
			case VRender::UNSIGNED_BYTE:
				converted = GL_RGBA8;
				break;
			case VRender::UNSIGNED_SHORT:
				converted = GL_RGBA16;
				break;
			case VRender::UNSIGNED_INT:
				converted = GL_RGBA32I;
				break;
			default:
				converted = GL_RGBA8;
				break;
			}

			target->renderBufferTypes.push_back(converted);
		}
	} else {
		target->renderBufferTypes.resize(bufferCount, GL_RGBA8);
	}

	target->Modify();
	deferredObjects.insert(target);
	UnLock();

	return target;
}

void ZRenderOpenGL3::BindOutput(VRender::Program* pro, Location index, VRender::Texture* tex) {
	CheckThread();
	ProgramImpl3* program = static_cast<ProgramImpl3*>(pro);
	TextureImpl3* texture = static_cast<TextureImpl3*>(tex);
	if (program->texturesForBuffer.size() <= (size_t)index) {
		program->texturesForBuffer.resize((int)(size_t)index + 1);
	}

	// assert(texture != nullptr);
	if (program->texturesForBuffer[(size_t)index] != texture) {
		program->texturesForBuffer[(size_t)index] = texture;
		// Not here!
		// program->outputBindStateChanged = true;
	}

	// Rebind it at FireRender!!
	// If not, diff texture with same target will not be updated.
	program->outputBindStateChanged = true;
}

void ZRenderOpenGL3::SetScreenSize(size_t width, size_t height) {
	screenWidth = width;
	screenHeight = height;
}

void ZRenderOpenGL3::GetTargetSize(VRender::Target* tar, size_t& width, size_t& height) {
	TargetImpl3* target = static_cast<TargetImpl3*>(tar);
	if (target != nullptr) {
		width = target->width;
		height = target->height;
	} else {
		width = screenWidth;
		height = screenHeight;
	}
}

/*
void ZRenderOpenGL3::CopyTarget(VRender::Target* t, const ZInt2Pair& toRect, size_t toIndex, VRender::Target* f, const ZInt2Pair& fromRect, size_t fromIndex) {
	CheckThread();
	GLErrorGuard guard;
	TargetImpl3* to = static_cast<TargetImpl3*>(t);
	TargetImpl3* from = static_cast<TargetImpl3*>(f);

	glBindFramebuffer(GL_FRAMEBUFFER, from == nullptr ? 0 : from->frameBuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, from == nullptr ? 0 : from->frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to == nullptr ? 0 : to->frameBuffer);

	glReadBuffer(from == nullptr ? GL_FRONT : GL_COLOR_ATTACHMENT0 + fromIndex);
	glDrawBuffer(to == nullptr ? GL_FRONT : GL_COLOR_ATTACHMENT0 + toIndex);

	glBlitFramebuffer(fromRect.first.x(), fromRect.first.y(), fromRect.second.x() - fromRect.first.x(), fromRect.second.y() - fromRect.first.y(),
		toRect.first.x(), toRect.first.y(), toRect.second.x() - toRect.first.x(), toRect.second.y() - toRect.first.y(),
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
*/

void ZRenderOpenGL3::ClearTarget(Target* t, const ZInt2Pair& rect, int m, const ZFloat4& color, float depth) {
	GLErrorGuard guard;
	CheckThread();
	TargetImpl3* target = static_cast<TargetImpl3*>(t);
	if (target != nullptr) {
		target->clearInfo.color = color;
		target->clearInfo.depth = depth;
		target->clearInfo.rect = rect;
		target->clearInfo.mask = m;
	} else {
		DoClear(rect, m, color, depth);
	}
}

/*
void ZRenderOpenGL3::ReadTarget(Target* t, size_t index, const ZInt2Pair& rect, VRender::Buffer* b) {
	CheckThread();
	GLErrorGuard guard;
	TargetImpl3* target = static_cast<TargetImpl3*>(t);
	BufferImpl3* buffer = static_cast<BufferImpl3*>(b);
	assert(buffer->bufferType == GL_PIXEL_PACK_BUFFER);

	if (target != nullptr) {
		// glBindFramebuffer(GL_FRAMEBUFFER, target->frameBuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, target->frameBuffer);
	}

	// int err = glGetError();
	if (index != DEPTH_BUFFER) {
		if (target == nullptr) {
			glReadBuffer(index == 0 ? GL_BACK : GL_AUX0 + index - 1);
		} else {
			glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
		}
	}

	// err = glGetError();
//	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, buffer->bufferID);
	glReadPixels(rect.first.x(), rect.first.y(), rect.second.x() - rect.first.x(), rect.second.y() - rect.first.y(), index == DEPTH_BUFFER ? GL_DEPTH_COMPONENT : GL_RGBA, GL_FLOAT, nullptr);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

//	glReadBuffer(GL_NONE);

	// err = glGetError();
	if (target != nullptr) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}*/

void ZRenderOpenGL3::DeleteTarget(VRender::Target* t) {
	TargetImpl3* target = static_cast<TargetImpl3*>(t);
	if (target != nullptr) {
		target->ReleaseObject();
		DoLock();
		deferredObjects.insert(target);
		UnLock();
	}
}

VRender::Location ZRenderOpenGL3::GetUniformBufferLocation(VRender::Program* program, const String& name) {
	CheckThread();
	GLErrorGuard guard;
	ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	return (Location)(size_t)glGetUniformBlockIndex(programHandle->programID, name.c_str());
}

VRender::Location ZRenderOpenGL3::GetOutputLocation(VRender::Program* program, const String& name) {
	CheckThread();
	GLErrorGuard guard;
	// printf("Location of %s = %d\n", name.c_str(), glGetFragDataLocation(programHandle->programID, name.c_str()));
	ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	return (Location)(size_t)glGetFragDataLocation(programHandle->programID, name.c_str());
}


VRender::Location ZRenderOpenGL3::GetUniformLocation(VRender::Program* program, const String& name) {
	CheckThread();
	GLErrorGuard guard;
	ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	return (Location)(size_t)glGetUniformLocation(programHandle->programID, name.c_str());
}

VRender::Location ZRenderOpenGL3::GetAttribLocation(VRender::Program* program, const String& name) {
	CheckThread();
	GLErrorGuard guard;
	ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	int location = glGetAttribLocation(programHandle->programID, name.c_str());
	if (location == -1) {
		location = glGetAttribLocation(programHandle->programID, (name + "_instanced").c_str());
	}
	return (Location)(size_t)location;
}

void ZRenderOpenGL3::BindUniform(VRender::Program* program, Location location, const float* value, size_t unitSize) {
	CheckThread();
	GLErrorGuard guard;
	// ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	assert((size_t)location != (size_t)-1);
	switch (unitSize) {
	case 1:
		glUniform1fv((int)(size_t)location, 1, value);
		break;
	case 2:
		glUniform2fv((int)(size_t)location, 1, value);
		break;
	case 3:
		glUniform3fv((int)(size_t)location, 1, value);
		break;
	case 4:
		glUniform4fv((int)(size_t)location, 1, value);
		break;
    default:
        break;
	}
//	assert(glGetError() == 0);
}

void ZRenderOpenGL3::BindUniform(VRender::Program* program, Location location, const int* value, size_t unitSize) {
	CheckThread();
	GLErrorGuard guard;
	// ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	assert((size_t)location != (size_t)-1);
	switch (unitSize) {
	case 1:
		glUniform1iv((int)(size_t)location, 1, value);
		break;
	case 2:
		glUniform2iv((int)(size_t)location, 1, value);
		break;
	case 3:
		glUniform3iv((int)(size_t)location, 1, value);
		break;
	case 4:
		glUniform4iv((int)(size_t)location, 1, value);
		break;
    default:
        break;
	}
//	assert(glGetError() == 0);
}

void ZRenderOpenGL3::BindUniform(VRender::Program* program, Location location, const ZMatrixFloat4x4& mat) {
	CheckThread();
	GLErrorGuard guard;
	assert((size_t)location != (size_t)-1);
	// ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	glUniformMatrix4fv((int)(size_t)location, 1, false, (const float*)&mat);
}

void ZRenderOpenGL3::BindUniform(VRender::Program* program, Location location, const ZMatrixFloat3x3& mat) {
	CheckThread();
	GLErrorGuard guard;
	assert((size_t)location != (size_t)-1);
	// ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	glUniformMatrix3fv((int)(size_t)location, 1, false, (const float*)&mat);
}


void ZRenderOpenGL3::BindUniform(VRender::Program* program, Location location, Texture* texture) {
	assert((size_t)location != (size_t)-1);
	GLErrorGuard guard;
	CheckThread();

	ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	TextureImpl3* textureHandle = static_cast<TextureImpl3*>(texture);
	std::map<VRender::Location, int>::iterator it = programHandle->mapLocationToTextures.find(location);
	size_t slotTexture;
	if (it == programHandle->mapLocationToTextures.end()) {
		// allocate new one
		slotTexture = programHandle->textures.size();
		programHandle->mapLocationToTextures[location] = (int)slotTexture;
		programHandle->textures.push_back(textureHandle);
	} else {
		slotTexture = it->second;
	}
	
	glActiveTexture((GLsizei)(GL_TEXTURE0 + slotTexture));
	glBindTexture(textureHandle->textureType, textureHandle->textureID);
	glUniform1i((int)(size_t)location, (int)slotTexture);
//	assert(glGetError() == 0);
}

void ZRenderOpenGL3::BindUniform(VRender::Program* program, Location location, Buffer* buffer, size_t offset, size_t size) {
	assert((size_t)location != (size_t)-1);
	GLErrorGuard guard;
	CheckThread();
	ProgramImpl3* programHandle = static_cast<ProgramImpl3*>(program);
	BufferImpl3* bufferHandle = static_cast<BufferImpl3*>(buffer);

	std::map<VRender::Location, int>::iterator it = programHandle->mapLocationToBuffers.find(location);
	size_t slotBuffer;
	if (it == programHandle->mapLocationToBuffers.end()) {
		// allocate new one
		slotBuffer = programHandle->buffers.size();
		programHandle->mapLocationToBuffers[location] = (GLsizei)slotBuffer;
		programHandle->buffers.push_back(bufferHandle);
	} else {
		slotBuffer = it->second;
	}

	assert(bufferHandle != nullptr);
	assert(bufferHandle->bufferType == GL_UNIFORM_BUFFER);
	// glBindBuffer(bufferHandle->bufferType, bufferHandle->bufferID);
	glBindBufferRange(bufferHandle->bufferType, (GLuint)slotBuffer, (GLuint)bufferHandle->bufferID, (GLsizei)offset, (GLsizei)(size == 0 ? bufferHandle->length : size));
	glUniformBlockBinding(programHandle->programID, (int)(size_t)location, (GLsizei)slotBuffer);
	// glBindBuffer(bufferHandle->bufferType, 0);
}

int ZRenderOpenGL3::FromBlendState(BLEND b) {
	int blend = GL_ONE;
	switch (b) {
	case ONE:
		blend = GL_ONE;
		break;
	case SRC_ALPHA:
		blend = GL_SRC_ALPHA;
		break;
	case ONE_MINUS_SRC_ALPHA:
		blend = GL_ONE_MINUS_SRC_ALPHA;
		break;
	case DST_ALPHA:
		blend = GL_DST_ALPHA;
		break;
	case ONE_MINUS_DST_ALPHA:
		blend = GL_ONE_MINUS_DST_ALPHA;
		break;
	}

	return blend;
}

void ZRenderOpenGL3::SetBlend(Program* pro, bool enable, BLEND src, BLEND dst) {
	ProgramImpl3* program = static_cast<ProgramImpl3*>(pro);
	program->enableBlend = enable;
	program->blendSrc = FromBlendState(src);
	program->blendDst = FromBlendState(dst);
}

void ZRenderOpenGL3::SetDepthTest(Program* pro, bool enable) {
	ProgramImpl3* program = static_cast<ProgramImpl3*>(pro);
	program->enableDepthTest = enable;
}

void ZRenderOpenGL3::SetDepthWrite(Program* pro, bool enable) {
	ProgramImpl3* program = static_cast<ProgramImpl3*>(pro);
	program->enableDepthWrite = enable;
}

void ZRenderOpenGL3::SetCullFace(Program* pro, bool enable) {
	ProgramImpl3* program = static_cast<ProgramImpl3*>(pro);
	program->enableCullFace = enable;
}

void ZRenderOpenGL3::FireRender(Program* pro) {
	GLErrorGuard guard;
	CheckThread();
	ProgramImpl3* program = static_cast<ProgramImpl3*>(pro);
	assert(program->vertexArrayBuffer != 0);
	assert(program->indexBuffer != nullptr);
	// glBindVertexArray(program->vertexArrayBuffer);
	assert(program->indexBuffer->bufferID != 0);
	// some drivers didn't restore it, so we call it manually.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, program->indexBuffer->bufferID);

	size_t to = program->to == 0 ? program->indexBuffer->length / sizeof(ZInt3) : program->to;
	size_t instanceCount = program->instanceCount;
	// glDisable(GL_BLEND);
	if (instanceCount == 0) {
		glDrawElements(GL_TRIANGLES, (GLsizei)(to - program->from) * 3, GL_UNSIGNED_INT, (const void*)(program->from * sizeof(ZInt3)));
	} else {
		glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)(to - program->from) * 3, GL_UNSIGNED_INT, (const void*)(program->from * sizeof(ZInt3)), (GLsizei)instanceCount);
	}
}

void ZRenderOpenGL3::SetPolygonMode(Program* program, POLYGON_MODE mode) {
	GLint m = mode == FILL ? GL_FILL : mode == POINT ? GL_POINT : GL_LINE;
	ProgramImpl3* programImpl = static_cast<ProgramImpl3*>(program);
	programImpl->polygonMode = m;
}

VRender::Notifier* ZRenderOpenGL3::CreateNotifier(const TWrapper<void, void*, VRender::NOTIFY_TYPE>& callback, NOTIFY_TYPE filter) {
	NotifierImpl3* notifier = new NotifierImpl3(callback, filter);
	return notifier;
}

void ZRenderOpenGL3::BindNotifierBuffer(Notifier* notifier, Buffer* bufferHandle) {
	NotifierImpl3* notifierImpl = static_cast<NotifierImpl3*>(notifier);
	BufferImpl3* bufferImpl = static_cast<BufferImpl3*>(bufferHandle);

	assert(notifierImpl != nullptr);
	assert(bufferImpl != nullptr);

	DoLock();
	notifierImpl->BindObject(bufferImpl);
	UnLock();
}

void ZRenderOpenGL3::BindNotifierTexture(Notifier* notifier, Texture* textureHandle) {
	NotifierImpl3* notifierImpl = static_cast<NotifierImpl3*>(notifier);
	TextureImpl3* textureImpl = static_cast<TextureImpl3*>(textureHandle);

	assert(notifierImpl != nullptr);
	assert(textureImpl != nullptr);

	DoLock();
	notifierImpl->BindObject(textureImpl);
	UnLock();
}

void ZRenderOpenGL3::UnBindNotifierBuffer(Notifier* notifier, Buffer* bufferHandle) {
	NotifierImpl3* notifierImpl = static_cast<NotifierImpl3*>(notifier);
	BufferImpl3* bufferImpl = static_cast<BufferImpl3*>(bufferHandle);

	assert(notifierImpl != nullptr);
	assert(bufferImpl != nullptr);

	DoLock();
	notifierImpl->UnBindObject(bufferImpl);
	UnLock();
}

void ZRenderOpenGL3::UnBindNotifierTexture(Notifier* notifier, Texture* textureHandle) {
	NotifierImpl3* notifierImpl = static_cast<NotifierImpl3*>(notifier);
	TextureImpl3* textureImpl = static_cast<TextureImpl3*>(textureHandle);

	assert(notifierImpl != nullptr);
	assert(textureImpl != nullptr);

	DoLock();
	notifierImpl->UnBindObject(textureImpl);
	UnLock();
}	

void ZRenderOpenGL3::DeleteNotifier(Notifier* notifier) {
	NotifierImpl3* notifierImpl = static_cast<NotifierImpl3*>(notifier);
	delete notifierImpl;
}

// Shader Compilation
static const String frameCode = "#version 330\n\
precision mediump float;\n\
precision mediump sampler2D;\n\
precision mediump samplerCube;\n\
#define _InstanceID gl_InstanceID \n\
#define lerp(a, b, v) mix(a, b, v) \n\
#define saturate(x) clamp(x, 0.0, 1.0) \n\
#define mult_mat(a, b) a * b; \n\
#define mult_vec(m, v) m * v; \n"
"vec4 _EncodeNormalDepth(vec4 normalDepth) { \n\
	float scale = 1.7777; \n\
	vec2 enc = normalDepth.xy / (normalDepth.z + 1); \n\
	enc /= scale; \n\
	vec2 encodeMul = vec2(1.0, 255.0); \n\
	float encodeBit = 1.0 / 255.0; \n\
	vec2 d = fract(encodeMul * normalDepth.w); \n\
	d.x -= d.y * encodeBit; \n\
	return vec4(enc * 0.5 + vec2(0.5, 0.5), d.x, d.y); \n\
}\n"
"vec4 _DecodeNormalDepth(vec4 data) { \n\
	float scale = 1.7777; \n\
	vec3 nn = vec3(data.xyz * vec3(2.0 * scale, 2.0 * scale, 0.0) + vec3(-scale, -scale, 1.0)); \n\
	float g = 2.0 / dot(nn.xyz, nn.xyz); \n\
	vec2 decodeMul = vec2(1.0, 1.0 / 255.0); \n\
	return vec4(g * nn.xy, g - 1.0, dot(decodeMul, data.zw)); \n\
}\n";

static String Declare(Unique typeID, bool array, const String& name, void* value, String& extra);

class ForeachDisp : public IReflect {
public:
	ForeachDisp() : IReflect(true, false) {}
	virtual void ProcessProperty(IReflectObject& s, Unique typeID, const char* name, void* base, void* ptr, const MetaChainBase* meta) {
		IShaderVariableBase& var = static_cast<IShaderVariableBase&>(s);
		String extra;
		result += String("\t") + Declare(var.GetSubUnique(), false, name, nullptr, extra) + ";\n";
	}

	virtual void ProcessMethod(Unique typeID, const char* name, const TProxy<>* p, const Param& retValue, const std::vector<Param>& params, const MetaChainBase* meta) {}

	String result;
};

#define DECALRE_VAR(type, keyword) \
	mapTypeNames[UniqueType<TShaderVariable<type, IShaderVariableBase::LOCAL> >::Get()] = #keyword; \
	mapTypeNames[UniqueType<TShaderVariable<type, IShaderVariableBase::INPUT_ATTRIB_BUFFER> >::Get()] = keyword; \
	mapTypeNames[UniqueType<TShaderVariable<type, IShaderVariableBase::INPUT_UNIFORM> >::Get()] = keyword; \
	mapTypeNames[UniqueType<TShaderVariable<type, IShaderVariableBase::INPUT_UNIFORM_TEXTURE> >::Get()] = "sampler2D"; \
	mapTypeNames[UniqueType<TShaderVariable<type, IShaderVariableBase::INPUT_INSTANCED_BUFFER> >::Get()] = keyword; \
	mapTypeNames[UniqueType<TShaderVariable<type, IShaderVariableBase::INPUT_VARYING> >::Get()] = keyword; \
	mapTypeNames[UniqueType<TShaderVariable<type, IShaderVariableBase::OUTPUT_POSITION> >::Get()] = keyword; \
	mapTypeNames[UniqueType<TShaderVariable<type, IShaderVariableBase::OUTPUT_VARYING> >::Get()] = keyword; \
	mapTypeNames[UniqueType<TShaderVariable<type, IShaderVariableBase::OUTPUT_TARGET> >::Get()] = keyword;

static String Declare(Unique typeID, bool array, const String& name, void* value, String& extra) {
	static std::map<Unique, String> mapTypeNames;
	static bool inited = false;
	if (!inited) {
		DECALRE_VAR(float, "float");
		DECALRE_VAR(ZFloat2, "vec2");
		DECALRE_VAR(ZFloat3, "vec3");
		DECALRE_VAR(ZFloat4, "vec4");
		DECALRE_VAR(ZMatrixFloat3x3, "mat3");
		DECALRE_VAR(ZMatrixFloat4x4, "mat4");
		inited = true;
	}

	if (typeID == UniqueType<IReflectObject*>::Get()) {
		IReflectObject* object = *reinterpret_cast<IReflectObject**>(value);
		assert(object != nullptr);
		ForeachDisp disp;
		(*object)(disp);
		extra = String("struct tag") + name + "_inner {\n" + disp.result + "};\n";
		return String("tag") + name + "_inner " + name;
	} else {
		return mapTypeNames[typeID] + (array ? "Array " : " ") + name;
	}
}

class ZShaderWalkerOpenGL : public ZShaderWalker {
public:
	ZShaderWalkerOpenGL(IShader& shader, VRender::SHADER_TYPE mainShaderType, std::vector<ShaderText>& shaderText) : ZShaderWalker(shader, mainShaderType, shaderText) {}
	virtual void Bind(Unique typeID, const String& rep, VAR_TYPE varType, bool array, bool flat, void* value, size_t count);
};

void ZShaderWalkerOpenGL::Bind(Unique typeID, const String& name, VAR_TYPE varType, bool array, bool flat, void* value, size_t count) {
	// declare variable
	bool skip = false;
	String extra;
	ShaderText& s = shaderText[(int)mainShaderType];
	String& headDeclare = s.head;
	String& bodyDeclare = s.body;
	std::map<String, String>& slots = s.slots;

	switch (varType) {
		case IShaderVariableBase::INPUT_ATTRIB_BUFFER:
		case IShaderVariableBase::INPUT_INSTANCED_BUFFER:
		case IShaderVariableBase::INPUT_VARYING:
		{
			if (flat)
				headDeclare += "flat ";
			headDeclare += String("in ") + Declare(typeID, array, name, nullptr, extra);

			// pass instanced vars ...
			if (mainShaderType == VRender::FRAGMENT_SHADER && varType == IShaderVariableBase::INPUT_INSTANCED_BUFFER) {
				ShaderText& v = shaderText[(int)VRender::VERTEX_SHADER];
				v.head += String("in ") + Declare(typeID, array, name + "_instanced", nullptr, extra) + ";\n";
				v.head += String("out ") + Declare(typeID, array, name, nullptr, extra) + ";\n";
				v.body += String("\t") + name + " = " + name + "_instanced;\n";
			}
		}
		break;
		case IShaderVariableBase::INPUT_UNIFORM:
		case IShaderVariableBase::INPUT_UNIFORM_TEXTURE:
		{
			headDeclare += String("uniform ") + Declare(typeID, array, name, nullptr, extra);
		}
		break;
		case IShaderVariableBase::INPUT_UNIFORM_BUFFER:
		{
			String decName = String("tag") + name;
			std::ostringstream os;
			os << count;
			Declare(typeID, array, name, value, extra);
			headDeclare += extra;
			headDeclare += String("uniform ") + name + " {\n\t" + Declare(typeID, array, name, value, extra) + "[" + os.str() + "];\n} " + decName;
			decName += String(".") + name;
		}
		break;
		case IShaderVariableBase::LOCAL:
		case IShaderVariableBase::OUTPUT_POSITION:
		{
			if (varType == IShaderVariableBase::OUTPUT_POSITION) {
				String& target = slots["gl_Position"];
				assert(target.empty());
				target = name;
			}

			bodyDeclare += String("\t") + Declare(typeID, array, name, nullptr, extra);
		}
		break;
		case IShaderVariableBase::OUTPUT_VARYING:
		case IShaderVariableBase::OUTPUT_TARGET:
		{
			if (flat) {
				headDeclare += "flat ";
			}

			headDeclare += String("out ") + Declare(typeID == UniqueType<VRender::Texture*>::Get() ? UniqueType<ZFloat4>::Get() : typeID, array, name, nullptr, extra);
		}
		break;
		default:
		{
			skip = true;
		}
		break;
	}

	if (!skip) {
		if (count != 0 && varType != IShaderVariableBase::INPUT_UNIFORM_BUFFER && varType != IShaderVariableBase::INPUT_INSTANCED_BUFFER) {
			std::ostringstream os;
			os << count;

			if (varType == IShaderVariableBase::LOCAL) {
				bodyDeclare += os.str();
			} else {
				headDeclare += os.str();
			}
		}

		if (varType == IShaderVariableBase::LOCAL || varType == IShaderVariableBase::OUTPUT_POSITION) {
			bodyDeclare += ";\n";
		} else {
			headDeclare += ";\n";
		}
	}
}

inline bool CompileShader(unsigned int shaderID, const String& shaderHead, const String& shaderBody, std::map<String, String>& slots) {
	GLErrorGuard guard;
	// generate head text
	static const String frameVersion = "#version 330\n\
precision mediump float;\n\
precision mediump sampler2D;\n\
precision mediump samplerCube;\n\
#define lerp(a, b, v) mix(a, b, v) \n\
#define saturate(x) clamp(x, 0.0, 1.0) \n\
#define mult_mat(a, b) a * b; \n\
#define mult_vec(m, v) m * v; \n"
"vec4 _EncodeNormalDepth(vec4 normalDepth) { \n\
	float scale = 1.7777; \n\
	vec2 enc = normalDepth.xy / (normalDepth.z + 1); \n\
	enc /= scale; \n\
	vec2 encodeMul = vec2(1.0, 255.0); \n\
	float encodeBit = 1.0 / 255.0; \n\
	vec2 d = fract(encodeMul * normalDepth.w); \n\
	d.x -= d.y * encodeBit; \n\
	return vec4(enc * 0.5 + vec2(0.5, 0.5), d.x, d.y); \n\
}\n"
"vec4 _DecodeNormalDepth(vec4 data) { \n\
	float scale = 1.7777; \n\
	vec3 nn = vec3(data.xyz * vec3(2.0 * scale, 2.0 * scale, 0.0) + vec3(-scale, -scale, 1.0)); \n\
	float g = 2.0 / dot(nn.xyz, nn.xyz); \n\
	vec2 decodeMul = vec2(1.0, 1.0 / 255.0); \n\
	return vec4(g * nn.xy, g - 1.0, dot(decodeMul, data.zw)); \n\
}\n";
	static const String frameHead = "void main(void) {\n";
	static const String frameTail = "}\n";

	String slotAssignments = "\n\n";
	for (std::map<String, String>::const_iterator it = slots.begin(); it != slots.end(); ++it) {
		slotAssignments += String("\t") + it->first + " = " + it->second + ";\n";
	}

	String body = frameHead + shaderBody + slotAssignments + frameTail;
	const char* src[] = { frameVersion.c_str(), shaderHead.c_str(), body.c_str() };
	glShaderSource(shaderID, sizeof(src) / sizeof(src[0]), src, nullptr);
	glCompileShader(shaderID);

	// printf("Compiling: %s\n\n", shaderText.c_str());

	int success;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (success == 0) {
		const int MAX_INFO_LOG_SIZE = 4096;
		char info[MAX_INFO_LOG_SIZE] = { 0 };
		glGetShaderInfoLog(shaderID, MAX_INFO_LOG_SIZE - 1, nullptr, info);
		fprintf(stderr, "ZRenderOpenGL3::CompileShader(): %s\n", info);
		fprintf(stderr, "ZRenderOpenGL3::CompileShader(): %s\n", body.c_str());
		//	MessageBox(nullptr, shaderText.c_str(), "TEXT", MB_OK);
		//	MessageBox(nullptr, info, "Shader Error", MB_OK);
		//	MessageBox(nullptr, shaderText.c_str(), "TEXT", MB_OK);
		assert(false);
		return false;
	} else {
		return true;
	}
}

VRender::Program* ZRenderOpenGL3::CreateProgram() {
	GLErrorGuard guard;
	ProgramImpl3* program = new ProgramImpl3();
	program->programID = glCreateProgram();
	program->outputBindStateChanged = true;
	program->enableDepthTest = true;
	program->enableDepthWrite = true;
	program->enableBlend = false;
	program->enableCullFace = true;
	program->blendSrc = GL_SRC_ALPHA;
	program->blendDst = GL_DST_ALPHA;
	program->polygonMode = GL_FILL;
	program->cacheTarget = nullptr;
	glGenVertexArrays(1, &program->vertexArrayBuffer);
	return program;
}

void ZRenderOpenGL3::DeleteProgram(VRender::Program* p) {
	ProgramImpl3* program = static_cast<ProgramImpl3*>(p);
	program->ReleaseObject();
	DoLock();
	deferredObjects.insert(program);
	UnLock();
}

void ZRenderOpenGL3::SetShaderSource(Program* p, SHADER_TYPE shaderType, IShader** shaderSource, size_t count, const String& preset) {
	ProgramImpl3* program = static_cast<ProgramImpl3*>(p);
	ProgramImpl3::Shader& s = program->shaders[(size_t)shaderType];
	s.config = preset;
	s.shaderPtrs.resize(count);
	memcpy(&s.shaderPtrs[0], shaderSource, sizeof(IShader*) * count);
}

bool ZRenderOpenGL3::CompileProgram(Program* p) {
	GLErrorGuard guard;
	ProgramImpl3* program = static_cast<ProgramImpl3*>(p);
	std::vector<ZShaderWalker::ShaderText> shaderTexts(sizeof(program->shaders) / sizeof(program->shaders[0]));

	for (size_t n = 0; n < sizeof(program->shaders) / sizeof(program->shaders[0]); n++) {
		ProgramImpl3::Shader& s = program->shaders[n];
		assert(!s.shaderPtrs.empty());
		assert(n == VRender::VERTEX_SHADER || n == VRender::FRAGMENT_SHADER);
		s.shaderID = glCreateShader(n == VRender::VERTEX_SHADER ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

		for (size_t k = 0; k < s.shaderPtrs.size(); k++) {
			ZShaderWalkerOpenGL walker(*(s.shaderPtrs[k]), (VRender::SHADER_TYPE)n, shaderTexts);
			walker.GenerateCode();
		}
	}

	for (size_t k = 0; k < shaderTexts.size(); k++) {
		ProgramImpl3::Shader& s = program->shaders[k];
		ZShaderWalker::ShaderText& text = shaderTexts[k];
		assert(!s.shaderPtrs.empty());
		if (!CompileShader(s.shaderID, text.head, text.body, text.slots)) {
			return false;
		}

		glAttachShader(program->programID, s.shaderID);
	}

	glLinkProgram(program->programID);
	int success;
	glGetProgramiv(program->programID, GL_LINK_STATUS, &success);
	if (success == 0) {
		const int MAX_INFO_LOG_SIZE = 4096;
		char info[MAX_INFO_LOG_SIZE] = { 0 };
		glGetProgramInfoLog(program->programID, MAX_INFO_LOG_SIZE - 1, nullptr, info);
		fprintf(stderr, "ZRenderOpenGL3::LinkProgram(): %s\n", info);
	//	MessageBox(nullptr, info, "Linker Error", MB_OK);
		assert(false);
		return false;
	} else {
		return true;
	}
}