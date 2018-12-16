#include "FontResource.h"
#include "../../../Core/Interface/IArchive.h"

using namespace PaintsNow;
using namespace PaintsNow::NsSnowyStream;

FontResource::FontResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID) : GraphicResourceBase(manager, uniqueID), font(nullptr), reinforce(0) {}
FontResource::~FontResource() {}

uint64_t FontResource::GetMemoryUsage() const {
	return 0;
}

bool FontResource::LoadExternalResource(IStreamBase& streamBase, size_t length) {
	rawFontData.resize(length);
	return streamBase.ReadBlock(const_cast<char*>(rawFontData.data()), length);
}

void FontResource::Attach(IRender& render) {
	GraphicResourceBase::Attach(render);
	
}

void FontResource::Detach(IRender& render) {
	for (std::map<uint32_t, Slice>::iterator it = sliceMap.begin(); it != sliceMap.end(); ++it) {
		it->second.Uninitialize(resourceManager);
	}
	sliceMap.clear();

	if (font != nullptr) {
		IFontBase& fontBase = resourceManager.GetInterfaces()->fontBase;
		fontBase.Close(font);
		font = nullptr;
	}

	GraphicResourceBase::Detach(render);
}

void FontResource::Upload(IRender& render) {
	IFontBase& fontBase = resourceManager.GetInterfaces()->fontBase;

	if (font == nullptr && !rawFontData.empty()) {
		// load font resource from memory
		ZMemoryStream ms(rawFontData.size(), false);
		size_t len = rawFontData.size();
		IFontBase& fontBase = resourceManager.GetInterfaces()->fontBase;
		if (ms.WriteBlock(rawFontData.data(), len)) {
			ms.Seek(IStreamBase::BEGIN, 0);
			font = fontBase.Load(ms, len);
		}

		if (font == nullptr) {
			// report error
			resourceManager.Report(String("Unable to load font resource: ") + uniqueLocation);
		}
	}
}

void FontResource::Download(IRender& render) {
}

TObject<IReflect>& FontResource::operator () (IReflect& reflect) {
	ReflectClass(FontResource)[Interface(GraphicResourceBase)];

	if (reflect.IsReflectProperty()) {
		// serializable
		ReflectProperty(rawFontData);
		ReflectProperty(reinforce);

		// runtime
		ReflectProperty(sliceMap)[Runtime];
		ReflectProperty(font)[Runtime];
	}

	return *this;
}

IRender::Texture* FontResource::GetFontTexture(uint32_t size, ZInt2& texSize) {
	std::map<uint32_t, Slice>::iterator it = sliceMap.find(size);
	if (it == sliceMap.end()) {
		return nullptr;
	} else {
		texSize = it->second.GetTextureSize();
		return it->second.GetFontTexture(resourceManager);
	}
}

const FontResource::Char& FontResource::Get(IFontBase::FONTCHAR ch, int32_t size) {
	size = size == 0 ? 12 : size;

	Slice& slice = sliceMap[size];
	if (slice.fontSize == 0) { // not initialized?
		slice.fontSize = size;
		slice.font = font;
		slice.Initialize(resourceManager);
	}

	return slice.Get(resourceManager, ch);
}

FontResource::Slice::Slice(uint32_t size = 0) : cacheTexture(nullptr), font(nullptr), fontSize(size), modified(false), width(512) {}

void FontResource::Slice::Initialize(ResourceManager& resourceManager) {
	IFontBase& fontBase = resourceManager.GetInterfaces()->fontBase;
	IRender& render = resourceManager.GetInterfaces()->render;
	assert(cacheTexture == nullptr);

	cacheTexture = render.CreateTexture(IRender::TEXTURE_2D, 1, IRender::COMPRESS_RGBA8, false, true);
}

void FontResource::Slice::Uninitialize(ResourceManager& resourceManager) {
	IFontBase& fontBase = resourceManager.GetInterfaces()->fontBase;
	IRender& render = resourceManager.GetInterfaces()->render;
	if (cacheTexture != nullptr) {
		render.DeleteTexture(cacheTexture);
		cacheTexture = nullptr;
	}
}

ZInt2Pair FontResource::Slice::AllocRect(const ZInt2& size) {
	assert(size.x() <= (int)width);
	if (lastRect.second.x() + size.x() > (int)width) {
		// new line
		lastRect.second.x() = 0;
		lastRect.first.y() = lastRect.second.y();
	}

	int height = Max(lastRect.second.y() - lastRect.first.y(), size.y());
	ZInt2Pair w;
	w.first.x() = lastRect.second.x();
	w.second.x() = w.first.x() + size.x();
	w.second.y() = lastRect.first.y() + height;
	w.first.y() = w.second.y() - size.y();
	lastRect = w;

	return w;
}

IRender::Texture* FontResource::Slice::GetFontTexture(ResourceManager& resourceManager) {
	if (modified) {
		UpdateFontTexture(resourceManager);
		modified = false;
	}

	return cacheTexture;
}

void FontResource::Slice::UpdateFontTexture(ResourceManager& resourceManager) {
	if (lastRect.second.y() != 0) {
		IRender& render = resourceManager.GetInterfaces()->render;
		render.WriteTexture(cacheTexture, 0, width, lastRect.second.y(), &buffer[0], IRender::LAYOUT_RGBA, IRender::UNSIGNED_BYTE);
	}
}

ZInt2 FontResource::Slice::GetTextureSize() const {
	return ZInt2((int)width, lastRect.second.y());
}

const FontResource::Char& FontResource::Slice::Get(ResourceManager& resourceManager, IFontBase::FONTCHAR ch) {
	hmap::iterator p = cache.find(ch);
	if (p != cache.end()) {
		return (*p).second;
	} else {
		IFontBase& fontBase = resourceManager.GetInterfaces()->fontBase;
		IRender& render = resourceManager.GetInterfaces()->render;
		assert(font != nullptr);
		Char c;
		String data;
		c.info = fontBase.RenderTexture(font, data, ch, fontSize, 0);
		c.rect = AllocRect(ZInt2(c.info.width, c.info.height));

		ZInt2Pair& r = c.rect;
		if ((int)buffer.size() < r.second.y() * width) {
			buffer.resize(r.second.y() * width);
		}

		const uint32_t* p = (const uint32_t*)data.data();
		for (int j = r.first.y(); j < r.second.y(); j++) {
			for (int i = r.first.x(); i < r.second.x(); i++) {
				buffer[j * width + i] = *p++;
				// printf("%02X ", (unsigned char)(buffer[j * width + i] >> 24));
			}
			// printf("\n");
		}
		// printf("\n");

		modified = true;
		return cache[ch] = c;
	}
}

