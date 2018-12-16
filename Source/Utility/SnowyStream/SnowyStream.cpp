#define _CRT_SECURE_NO_WARNINGS
#include "SnowyStream.h"
#include "Resource/AudioResource.h"
#include "Resource/FontResource.h"
#include "Resource/MaterialResource.h"
#include "Resource/MeshResource.h"
#include "Resource/SkeletonResource.h"
#include "Resource/TapeResource.h"
#include "Resource/TextResource.h"
#include "Resource/TextureResource.h"
#include "Resource/Pipelines/StandardPipeline.h"
#include "Resource/Pipelines/ParticlePipeline.h"
#include "Resource/Pipelines/TerrainPipeline.h"
#include "Resource/Pipelines/VolumePipeline.h"
#include "Resource/Pipelines/WaterPipeline.h"
#include <iterator>

using namespace PaintsNow;
using namespace NsSnowyStream;

String SnowyStream::reflectedExtension = "*.rds";

SnowyStream::SnowyStream(Interfaces& inters, const TWrapper<IArchive*, IStreamBase&, size_t>& psubArchiveCreator, const TWrapper<void, const String&>& err) : Library(inters.thread), interfaces(inters), errorHandler(err), subArchiveCreator(psubArchiveCreator) {
	RegisterReflectedSerializers();
	RegisterBuiltinPipelines();
}

bool SnowyStream::Initialize() {
	CreateBuiltinResources();
	return true;
}

SnowyStream::~SnowyStream() {}

Interfaces& SnowyStream::GetInterfaces() const {
	return interfaces;
}

void SnowyStream::Reset() {
}


TObject<IReflect>& SnowyStream::operator () (IReflect& reflect) {
	ReflectClass(SnowyStream)[Interface(IScript::Library)];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(interfaces)[Runtime];
		ReflectProperty(errorHandler)[Runtime];
		ReflectProperty(resourceSerializers)[Runtime];
		ReflectProperty(resourceManagers)[Runtime];
	}

	if (reflect.IsReflectMethod()) {
		ReflectMethod(RequestCreateResource)[ScriptMethod = "CreateResource"];
		ReflectMethod(RequestLoadExternalResourceData)[ScriptMethod = "LoadExternalResourceData"];
		ReflectMethod(RequestPersistResource)[ScriptMethod = "PersistResource"];
		ReflectMethod(RequestGetResourceInfo)[ScriptMethod = "GetResourceInfo"];
		ReflectMethod(RequestCreateFile)[ScriptMethod = "CreateFile"];
		ReflectMethod(RequestFlushFile)[ScriptMethod = "FlushFile"];
		ReflectMethod(RequestReadFile)[ScriptMethod = "ReadFile"];
		ReflectMethod(RequestGetFileSize)[ScriptMethod = "GetFileSize"];
		ReflectMethod(RequestCloseFile)[ScriptMethod = "CloseFile"];
		ReflectMethod(RequestGetFileLastModifiedTime)[ScriptMethod = "GetFileLastModifiedTime"];
		ReflectMethod(RequestWriteFile)[ScriptMethod = "WriteFile"];
		ReflectMethod(RequestSeekFile)[ScriptMethod = "SeekFile"];
		ReflectMethod(RequestQueryFiles)[ScriptMethod = "QueryFiles"];
		ReflectMethod(RequestQuickTextResource)[ScriptMethod = "QuickTextResource"];
		ReflectMethod(RequestFileExists)[ScriptMethod = "FileExists"];
		ReflectMethod(RequestCreateZipper)[ScriptMethod = "CreateZipper"];
		ReflectMethod(RequestPostZipperData)[ScriptMethod = "PostZipperData"];
		ReflectMethod(RequestWriteZipper)[ScriptMethod = "WriteZipper"];
	}

	return *this;
}

void SnowyStream::RequestCreateZipper(IScript::Request& request, String& path) {
	METHOD_ENTRY(SnowyStream::RequestCreateZipper);
	if (subArchiveCreator) {
		size_t length;
		IStreamBase* stream = interfaces.archive.Open(path, false, length);
		if (stream != nullptr) {
			IArchive* a = subArchiveCreator(*stream, length);
			if (a != nullptr) {
				Zipper* zipper = new Zipper(a, stream);
				request.DoLock();
				request << delegate(zipper);
				request.UnLock();
				zipper->ReleaseObject();
			} else {
				delete a;
				request.Error("SnowyStream.CreateZipper(): Cannot open stream.");
			}
		} else {
			request.Error("SnowyStream.CreateZipper(): Cannot open archive.");
		}
	} else {
		request.Error("SnowyStream.CreateZipper(): No archive creator found.");
	}
}

void SnowyStream::RequestPostZipperData(IScript::Request& request, IScript::Delegate<Zipper> zipper, String& path, String& data) {
	METHOD_ENTRY(SnowyStream::RequestPostZipperData);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(zipper);

	// TODO:
}

void SnowyStream::RequestWriteZipper(IScript::Request& request, IScript::Delegate<File> file, IScript::Delegate<Zipper> zipper) {
	METHOD_ENTRY(SnowyStream::RequestWriteZipper);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(file);
	CHECK_DELEGATE(zipper);

	// TODO:
}

void SnowyStream::RequestFileExists(IScript::Request& request, String& path) {
	METHOD_ENTRY(SnowyStream::RequestFileExists);

	IArchive& archive = interfaces.archive;
	size_t length;
	IStreamBase* stream = archive.Open(path, false, length);
	bool ret = stream != nullptr;
	if (ret) {
		stream->ReleaseObject();
	} else {
		size_t length;
		ret = request.GetScript()->QueryUniformResource(path, length) != nullptr;
	}

	request.DoLock();
	request << ret;
	request.UnLock();
}


void SnowyStream::RequestQuickTextResource(IScript::Request& request, String& path) {
	METHOD_ENTRY(SnowyStream::RequestQuickTextResource);
	size_t length;
	IArchive& archive = interfaces.archive;
	IStreamBase* stream = archive.Open(path, false, length);
	bool success = false;
	if (stream != nullptr) {
		String buf;
		buf.resize(length);
		if (stream->Read(const_cast<char*>(buf.data()), length)) {
			request.DoLock();
			request << buf;
			request.UnLock();
		}

		stream->ReleaseObject();
	} else {
		const char* builtin = request.GetScript()->QueryUniformResource(path, length);
		if (builtin != nullptr) {
			request.DoLock();
			request << String(builtin, length);
			request.UnLock();
		}
	}
}

void SnowyStream::RequestCloseFile(IScript::Request& request, IScript::Delegate<File> file) {
	METHOD_ENTRY(SnowyStream::RequestCloseFile);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(file);
	file->Close();
}

void SnowyStream::RequestCreateFile(IScript::Request& request, String& path, bool write) {
	METHOD_ENTRY(SnowyStream::RequestCreateFile);
	CHECK_REFERENCES_NONE();
	IArchive& archive = interfaces.archive;
	size_t length = 0;
	uint64_t modifiedTime = 0;
	IStreamBase* stream = path == ":memory:" ? new ZMemoryStream(0x1000, true) : archive.Open(path, write, length, &modifiedTime);
	if (stream != nullptr) {
		TShared<File> file = TShared<File>::Make(stream, length, modifiedTime);
		request.DoLock();
		request << delegate(file());
		request.UnLock();
	}
}

void SnowyStream::RequestGetFileLastModifiedTime(IScript::Request& request, IScript::Delegate<File> file) {
	METHOD_ENTRY(SnowyStream::RequestGetFileLastModifiedTime);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(file);
	uint64_t lastModifiedTime = file->GetLastModifiedTime();

	request.DoLock();
	request << lastModifiedTime;
	request.UnLock();
}

void SnowyStream::RequestGetFileSize(IScript::Request& request, IScript::Delegate<File> file) {
	METHOD_ENTRY(SnowyStream::RequestGetFileSize);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(file);
	size_t length = file->GetLength();

	request.DoLock();
	request << length;
	request.UnLock();
}

void SnowyStream::RequestReadFile(IScript::Request& request, IScript::Delegate<File> file, int64_t length, IScript::Request::Ref callback) {
	METHOD_ENTRY(SnowyStream::RequestReadFile);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(file);
	if (callback) {
		CHECK_REFERENCES_WITH_TYPE(callback, IScript::Request::FUNCTION);
	}
	IStreamBase* stream = file->GetStream();
	if (stream != nullptr) {
		String content;
		size_t len = (size_t)length;
		content.resize(len);
		if (stream->Read(const_cast<char*>(content.data()), len)) {
			request.DoLock();
			if (callback) {
				request.Call(Async, callback, content);
			} else {
				request << content;
			}
			request.UnLock();
		}
	} else {
		request.Error("SnowyStream::ReadFile() : File already closed.");
	}
}

void SnowyStream::RequestWriteFile(IScript::Request& request, IScript::Delegate<File> file, String& content, IScript::Request::Ref callback) {
	METHOD_ENTRY(SnowyStream::RequestWriteFile);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(file);
	if (callback) {
		CHECK_REFERENCES_WITH_TYPE(callback, IScript::Request::FUNCTION);
	}

	IStreamBase* stream = file->GetStream();
	if (stream != nullptr) {
		size_t len = content.size();
		if (stream->Write(content.data(), len)) {
			request.DoLock();
			if (callback) {
				request.Call(Async, callback, len);
			} else {
				request << len;
			}
			request.UnLock();
		}
	} else {
		request.Error("SnowyStream::WriteFile() : File already closed.");
	}
}

void SnowyStream::RequestFlushFile(IScript::Request& request, IScript::Delegate<File> file) {
	METHOD_ENTRY(SnowyStream::RequestFlushFile);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(file);
	IStreamBase* stream = file->GetStream();
	if (stream != nullptr) {
		stream->Flush();
	} else {
		request.Error("SnowyStream::FlushFile() : File already closed.");
	}
}


void SnowyStream::RequestSeekFile(IScript::Request& request, IScript::Delegate<File> file, String& type, int64_t offset) {
	METHOD_ENTRY(SnowyStream::RequestSeekFile);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(file);
	IStreamBase* stream = file->GetStream();
	if (stream != nullptr) {
		IStreamBase::SEEK_OPTION option = IStreamBase::CUR;
		if (type == "Current") {
			option = IStreamBase::CUR;
		} else if (type == "Begin") {
			option = IStreamBase::BEGIN;
		} else if (type == "End") {
			option = IStreamBase::END;
		}

		if (stream->Seek(option, (long)offset)) {
			request.DoLock();
			request << true;
			request.UnLock();
		}
	} else {
		request.Error("SnowyStream::SeekFile() : File already closed.");
	}
}

struct QueryHandler {
	QueryHandler(const String& p, IScript::Request& r) : prefix(p), request(r) {}
	void Accept(bool isDir, const String& path) {
		String t = path;
		if (isDir) t += "/";
		request << t;
	}

	const String prefix;
	IScript::Request& request;
};

bool SnowyStream::FilterPath(const String& path) {
	return path.find("..") == String::npos;
}

void SnowyStream::RequestQueryFiles(IScript::Request& request, String& path) {
	METHOD_ENTRY(SnowyStream::RequestQueryFiles);
	if (!FilterPath(path)) return;
	char ch = path[path.length() - 1];
	if (ch != '/' && ch != '\\') {
		path += "/";
	}

	IArchive& archive = interfaces.archive;
	QueryHandler handler(path, request);
	request.DoLock();
	request << begintable;
	archive.Query(path, Wrap(&handler, &QueryHandler::Accept));
	request << endtable;
	request.UnLock();
}

void SnowyStream::RequestCreateResource(IScript::Request& request, String& expectedResType, String& path, bool createAlways) {
	METHOD_ENTRY(SnowyStream::RequestCreateResource);

	ResourceBase* resource = CreateResource(path, expectedResType, !createAlways);

	request.DoLock();
	if (resource != nullptr) {
		request << delegate(resource);
	} else {
		request.Error(String("Unable to create resource ") + path);
	}
	request.UnLock();

	resource->ReleaseObject();
}

void SnowyStream::RequestLoadExternalResourceData(IScript::Request& request, IScript::Delegate<ResourceBase> resource, String& externalData) {
	METHOD_ENTRY(SnowyStream::RequestLoadExternalResourceData);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(resource);

	ZMemoryStream ms(externalData.size());
	size_t len = externalData.size();
	ms.Write(externalData.c_str(), len);
	ms.Seek(IStreamBase::BEGIN, 0);
	bool result = resource->LoadExternalResource(ms, len);
	if (result) {
		resource->GetResourceManager().InvokeUpload(resource.Get());
	}

	request.DoLock();
	request << result;
	request.UnLock();
}

void SnowyStream::RequestGetResourceInfo(IScript::Request& request, IScript::Delegate<ResourceBase> resource) {
	METHOD_ENTRY(SnowyStream::RequestGetResourceInfo);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(resource);

	Tiny::FLAG flag = resource->Flag().load();
	std::vector<TShared<ResourceBase> > dependencies;
	resource->GetDependencies(dependencies);

	request.DoLock();
	request << begintable
		<< key("Flag") << flag
		<< key("Depends") << begintable;

	for (size_t k = 0; k < dependencies.size(); k++) {
		request << delegate(dependencies[k]());
	}

		request << endtable << endtable;
	request.UnLock();
}

void SnowyStream::RequestPersistResource(IScript::Request& request, IScript::Delegate<ResourceBase> resource, String& extension) {
	METHOD_ENTRY(SnowyStream::RequestPersistResource);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(resource);

	bool result = PersistResource(resource.Get(), extension);
	request.DoLock();
	request << result;
	request.UnLock();
}

void SnowyStream::Uninitialize() {
	std::map<Unique, TShared<ResourceManager> > temp;
	std::swap(temp, resourceManagers);

	for (std::map<Unique, TShared<ResourceManager> >::const_iterator p = temp.begin(); p != temp.end(); ++p) {
		(*p).second->RemoveAll();
	}
}

template <class T>
void RegisterPipeline(ResourceManager& resourceManager, UniqueType<T> type) {
	ShaderResourceImpl<T>* shaderResource = new ShaderResourceImpl<T>(resourceManager, "");
	Unique unique = shaderResource->GetPipeline().GetUnique();
	assert(unique != UniqueType<ZPipeline>().Get());
	String name = shaderResource->GetPipeline().GetUnique()->typeName;
	auto pos = name.find_last_of(':');
	if (pos != std::string::npos) {
		name = name.substr(pos + 1);
	}

	shaderResource->SetLocation(ShaderResource::GetShaderPathPrefix() + name);
	shaderResource->Flag() |= ResourceBase::RESOURCE_ETERNAL;
	resourceManager.Insert(shaderResource->GetLocation(), shaderResource);
}

void SnowyStream::RegisterBuiltinPipelines() {
	TShared<ResourceManager> resourceManager = resourceManagers[UniqueType<IRender>::Get()];
	assert(resourceManager);
	
	RegisterPipeline(*resourceManager(), UniqueType<StandardPipeline>());
	/*
	RegisterPipeline(*resourceManager(), UniqueType<ParticlePipeline>());
	RegisterPipeline(*resourceManager(), UniqueType<TerrainPipeline>());
	RegisterPipeline(*resourceManager(), UniqueType<VolumePipeline>());
	RegisterPipeline(*resourceManager(), UniqueType<WaterPipeline>());*/
}

void SnowyStream::RegisterReflectedSerializers() {
	assert(resourceSerializers.empty()); // can only be registerred once

	// Commented resources may have dependencies, so we do not serialize/deserialize them at once.
	// PaintsNow recommends database-managed resource dependencies ...

	RegisterReflectedSerializer(UniqueType<AudioResource>(), interfaces.audio);
	RegisterReflectedSerializer(UniqueType<FontResource>(), interfaces.render);
	// RegisterReflectedSerializer(UniqueType<MateriaResource>(), interfaces.render);
	RegisterReflectedSerializer(UniqueType<ShaderResource>(), interfaces.render);
	// RegisterReflectedSerializer(UniqueType<ModelResource>(), interfaces.render);
	RegisterReflectedSerializer(UniqueType<MeshResource>(), interfaces.render);
	RegisterReflectedSerializer(UniqueType<SkeletonResource>(), interfaces.render);
	RegisterReflectedSerializer(UniqueType<TextureResource>(), interfaces.render);
	RegisterReflectedSerializer(UniqueType<TapeResource>(), interfaces.archive);
	RegisterReflectedSerializer(UniqueType<TextResource>(), interfaces.archive);
}

void SnowyStream::ScriptUninitialize(IScript::Request& request) {
}

bool SnowyStream::RegisterResourceManager(Unique unique, ResourceManager* resourceManager) {
	if (resourceManagers.count(unique) == 0) {
		resourceManagers[unique] = resourceManager;
		return true;
	} else {
		return false;
	}
}

bool SnowyStream::RegisterResourceSerializer(Unique unique, const String& extension, ResourceSerializerBase* serializer) {
	if (resourceSerializers.count(extension) == 0) {
		resourceSerializers[extension] = std::make_pair(unique, serializer);
		return true;
	} else {
		return false;
	}
}

ResourceBase* SnowyStream::CreateResource(const String& location, const String& typeExtension, bool openExisting, IStreamBase* sourceStream) {
	// Find resource serializer
	unordered_map<String, std::pair<Unique, TShared<ResourceSerializerBase> > >::iterator p = resourceSerializers.find(typeExtension);
	IArchive& archive = interfaces.archive;
	IFilterBase& protocol = interfaces.filterBase;

	if (p != resourceSerializers.end()) {
		// query manager
		std::map<Unique, TShared<ResourceManager> >::iterator t = resourceManagers.find((*p).second.first);
		assert(t != resourceManagers.end());
		if (sourceStream == nullptr) {
			return (*p).second.second->DeserializeFromArchive(*(*t).second(), archive, location, protocol, openExisting);
		} else {
			assert(!openExisting);
			return (*p).second.second->Deserialize(*(*t).second(), location, protocol, sourceStream);
		}
	}

	return nullptr;
}

String SnowyStream::GetReflectedExtension(Unique unique) {
	return unique->GetSubName();
}

bool SnowyStream::PersistResource(ResourceBase* resource, const String& extension) {
	// Find resource serializer
	assert(resource != nullptr);
	String typeExtension = extension.empty() ? GetReflectedExtension(resource->GetUnique()) : extension;
	IArchive& archive = interfaces.archive;
	IFilterBase& protocol = interfaces.filterBase;

	unordered_map<String, std::pair<Unique, TShared<ResourceSerializerBase> > >::iterator p = resourceSerializers.find(typeExtension);
	if (p != resourceSerializers.end()) {
		// query manager
		std::map<Unique, TShared<ResourceManager> >::iterator t = resourceManagers.find((*p).second.first);
		assert(t != resourceManagers.end());
		return (*p).second.second->SerializeToArchive(resource, archive, protocol, resource->GetLocation());
	} else {
		return false;
	}
}

void SnowyStream::CreateBuiltinResources() {
	// MeshResource for widget rendering and deferred rendering ...
	MeshResource* meshResource = CreateReflectedResource(UniqueType<MeshResource>(), "[Runtime]/MeshResource/StandardSquare", false);
	IRender& render = interfaces.render;

	if (meshResource != nullptr) {
		static const ZFloat3 vertices[] = {
			ZFloat3(0.0f, 0.0f, 0.0f),
			ZFloat3(1.0f, 0.0f, 0.0f),
			ZFloat3(1.0f, 1.0f, 0.0f),
			ZFloat3(0.0f, 1.0f, 0.0f),
		};

		/*
		static const ZFloat4 texCoords[] = {
			ZFloat4(0.0f, 0.0f, 0.0f, 0.0f),
			ZFloat4(1.0f, 0.0f, 1.0f, 0.0f),
			ZFloat4(1.0f, 1.0f, 1.0f, 1.0f),
			ZFloat4(0.0f, 1.0f, 0.0f, 1.0f)
		};*/

		static const ZInt3 indices[] = { ZInt3(0, 1, 2), ZInt3(2, 3, 0) };
		// No UVs
		std::copy(vertices, vertices + sizeof(vertices) / sizeof(vertices[0]), std::back_inserter(meshResource->meshCollection.vertices));
		std::copy(indices, indices + sizeof(indices) / sizeof(indices[0]), std::back_inserter(meshResource->meshCollection.indices));
		meshResource->Upload(render);
		meshResource->Flag() |= ResourceBase::RESOURCE_ETERNAL;
	} else {
		errorHandler("Unable to create builtin mesh ...");
	}

	// Error Texture for missing textures ...
	TextureResource* errorTexture = CreateReflectedResource(UniqueType<TextureResource>(), "[Runtime]/TextureResource/Missing", false);
	if (errorTexture != nullptr) {
		errorTexture->dataType = IRender::UNSIGNED_BYTE;
		errorTexture->layout = IRender::LAYOUT_RGBA;
		// 2x2 pixel
		const int width = 2, height = 2;
		errorTexture->width = width;
		errorTexture->height = height;
		errorTexture->textureData.resize(width * height * sizeof(ZUChar4));
		
		const ZUChar4 color(255, 0, 255, 255);
		ZUChar4* buffer = reinterpret_cast<ZUChar4*>(const_cast<char*>(&errorTexture->textureData[0]));
		for (size_t i = 0; i < width * height; i++) {
			buffer[i] = color;
		}

		errorTexture->Upload(render);
		errorTexture->Flag() |= ResourceBase::RESOURCE_ETERNAL;
	} else {
		errorHandler("Unable to create error texture ...");
	}
}
