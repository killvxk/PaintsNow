#include "RenderTargetResource.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

RenderTargetResource::RenderTargetResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : GraphicResourceBase(manager, uniqueID), renderTarget(nullptr), width(256), height(256), dataFormat(1, IRender::UNSIGNED_BYTE) {}

uint64_t RenderTargetResource::GetMemoryUsage() const {
	return 0;
}

void RenderTargetResource::Attach(IRender& render) {
	GraphicResourceBase::Attach(render);

	renderTarget = render.CreateTarget(width, height, dataFormat.size(), &dataFormat[0]);
}

void RenderTargetResource::Detach(IRender& render) {
	GraphicResourceBase::Detach(render);

	render.DeleteTarget(renderTarget);
}

void RenderTargetResource::Upload(IRender& render) {
	// render.WriteRenderTarget(instance, 0, width, height, textureData.data(), layout, dataType);
}

void RenderTargetResource::Download(IRender& render) {
	// render.ReadRenderTarget(instance, 0, const_cast<char*>(textureData.data()), dataType);
}

TObject<IReflect>& RenderTargetResource::operator () (IReflect& reflect) {
	ReflectClass(RenderTargetResource)[Interface(GraphicResourceBase)];

	if (reflect.IsReflectProperty()) {
		ReflectProperty(width);
		ReflectProperty(height);
		ReflectProperty(IterateVector(dataFormat));
	}

	return *this;
}

IRender::Target* RenderTargetResource::GetRenderTarget() const {
	return renderTarget;
}