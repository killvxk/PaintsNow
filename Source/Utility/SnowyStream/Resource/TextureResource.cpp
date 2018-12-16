#include "TextureResource.h"
#include "../../../Core/Interface/IImage.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

TextureResource::TextureResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : GraphicResourceBase(manager, uniqueID), instance(nullptr), dataType(IRender::UNSIGNED_BYTE), layout(IRender::LAYOUT_RGBA), width(0), height(0) {}

uint64_t TextureResource::GetMemoryUsage() const {
	return 0;
}

void TextureResource::Attach(IRender& render) {
	GraphicResourceBase::Attach(render);
	instance = render.CreateTexture(IRender::TEXTURE_2D, 1, IRender::COMPRESS_RGBA8);

	assert(statusNotifier != nullptr);
	render.BindNotifierTexture(statusNotifier, instance);
}

void TextureResource::Detach(IRender& render) {
	if (instance != nullptr) {
		render.DeleteTexture(instance);
		render.UnBindNotifierTexture(statusNotifier, instance);
	}
	GraphicResourceBase::Detach(render);
}

void TextureResource::Upload(IRender& render) {
	assert(textureData.size() == width * height * IImage::GetPixelSize(dataType, layout));
	render.WriteTexture(instance, 0, width, height, textureData.data(), layout, dataType);
	if (!(Flag() & RESOURCE_RESERVE_LOCALDATA))
		textureData = "";
}

void TextureResource::Download(IRender& render) {
	textureData.resize(width * height * IImage::GetPixelSize(dataType, layout));
	render.ReadTexture(instance, 0, const_cast<char*>(textureData.data()), dataType);
}

IRender::Texture* TextureResource::GetTexture() const {
	return instance;
}

TObject<IReflect>& TextureResource::operator () (IReflect& reflect) {
	ReflectClass(TextureResource)[Interface(GraphicResourceBase)];
	if (reflect.IsReflectProperty()) {
		// serializable
		ReflectProperty(dataType);
		ReflectProperty(layout);
		ReflectProperty(width);
		ReflectProperty(height);
		ReflectProperty(textureData);

		// runtime
		ReflectProperty(instance)[Runtime];
	}

	return *this;
}

bool TextureResource::LoadExternalResource(IStreamBase& streamBase, size_t length) {
	Flag() |= ResourceBase::RESOURCE_RESERVE_LOCALDATA;

	IImage& imageBase = resourceManager.GetInterfaces()->image;
	IImage::Image* image = imageBase.Create(1, 1, IRender::LAYOUT_RGB, IRender::DATA_TYPE::UNSIGNED_BYTE);
	if (image == nullptr) return false;
	bool success = imageBase.Load(image, streamBase, length);
	layout = imageBase.GetLayoutType(image);
	dataType = imageBase.GetDataType(image);
	width = (uint16_t)imageBase.GetWidth(image);
	height = (uint16_t)imageBase.GetHeight(image);
	void* buffer = imageBase.GetBuffer(image);
	textureData.assign(reinterpret_cast<char*>(buffer), width * height * IImage::GetPixelSize(dataType, layout));
	// copy info
	imageBase.Delete(image);
	return success;
}