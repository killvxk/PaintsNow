// FontResource.h
// By PaintDream (paintdream@paintdream.com)
// 2018-3-11
//

#ifndef __FONTRESOURCE_H__
#define __FONTRESOURCE_H__

#include "GraphicResourceBase.h"
#include "../../../Core/Interface/IFontBase.h"

namespace PaintsNow {
	namespace NsSnowyStream {
		class FontResource : public GraphicResourceBase {
		public:
			FontResource(ResourceManager& manager, const ResourceManager::UniqueLocation& uniqueID);
			virtual ~FontResource();
			virtual uint64_t GetMemoryUsage() const;
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual bool LoadExternalResource(IStreamBase& streamBase, size_t length);

			virtual void Upload(IRender& render);
			virtual void Download(IRender& render);
			virtual void Attach(IRender& render);
			virtual void Detach(IRender& render);

			struct Char {
				Char() { memset(this, 0, sizeof(*this)); }
				IFontBase::CHARINFO info;
				ZInt2Pair rect;
			};

			const Char& Get(IFontBase::FONTCHAR ch, int32_t size);
			IRender::Texture* GetFontTexture(uint32_t size, ZInt2& texSize);

		protected:
			class Slice {
			public:
				Slice(uint32_t size);
				const Char& Get(ResourceManager& resourceManager, IFontBase::FONTCHAR ch);
				void Initialize(ResourceManager& resourceManager);
				void Uninitialize(ResourceManager& resourceManager);
				IRender::Texture* GetFontTexture(ResourceManager& resourceManager);

				friend class FontResource;

			protected:
				typedef unordered_map<IFontBase::FONTCHAR, Char> hmap;
				hmap cache;
				ZInt2Pair lastRect;
				std::vector<uint32_t> buffer;
				int16_t width;
				uint16_t fontSize;
				IFontBase::Font* font;
				IRender::Texture* cacheTexture;
				bool modified;

				ZInt2 GetTextureSize() const;
				void UpdateFontTexture(ResourceManager& resourceManager);
				ZInt2Pair AllocRect(const ZInt2& size);
			};

		private:
			std::map<uint32_t, Slice> sliceMap;
			IFontBase::Font* font;
			String rawFontData;
			float reinforce;
		};
	}
}


#endif // __FONTRESOURCE_H__
