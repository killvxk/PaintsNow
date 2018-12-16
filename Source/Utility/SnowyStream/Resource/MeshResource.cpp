#include "MeshResource.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

MeshResource::MeshResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : GraphicResourceBase(manager, uniqueID) {}

MeshResource::~MeshResource() {}

uint64_t MeshResource::GetMemoryUsage() const {
	return 0;
}

bool MeshResource::operator << (IStreamBase& stream) {
	return false;
}

bool MeshResource::operator >> (IStreamBase& stream) {
	return false;
}

void MeshResource::Attach(IRender& render) {
	GraphicResourceBase::Attach(render);
}

void MeshResource::Detach(IRender& render) {
	// delete buffers ...
	meshCollection = IAsset::MeshCollection();
	Upload(render);

	GraphicResourceBase::Detach(render);
}

void MeshResource::Upload(IRender& render) {
	// Update buffers ...
	Flag() &= ~RESOURCE_UPLOADED;

	UpdateBuffer(render, bufferCollection.indexBuffer, meshCollection.indices, IRender::INDEX);
	UpdateBuffer(render, bufferCollection.positionBuffer, meshCollection.vertices);
	UpdateBuffer(render, bufferCollection.normalBuffer, meshCollection.normals);
	UpdateBuffers(render, bufferCollection.texCoordBuffers, meshCollection.texCoords);
}

void MeshResource::Download(IRender& render) {
	// download from device is not supported now
	assert(false);
}

// BufferCollection
MeshResource::BufferCollection::BufferCollection() : indexBuffer(nullptr), positionBuffer(nullptr), normalBuffer(nullptr), vertexColorBuffer(nullptr) {}


TObject<IReflect>& MeshResource::operator () (IReflect& reflect) {
	ReflectClass(MeshResource)[Interface(GraphicResourceBase)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(meshCollection);
		ReflectProperty(bufferCollection)[Runtime];
	}

	return *this;
}