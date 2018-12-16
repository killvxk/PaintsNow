// Widget.h
// By PaintDream (paintdream@paintdream.com)
// 2015-1-1
//

#ifndef __WIDGET_H__
#define __WIDGET_H__

#include "../../Core/Object/Tiny.h"
#include "TextControl.h"
#include "Message.h"
#include <string>
#include <set>

namespace PaintsNow {
	namespace NsSnowyStream {
		class TextureResource;
		class FontResource;
		class WidgetPipeline;
	}

	namespace NsLunaWidget {
		class Widget : public SharedTiny {
		public:
			enum {
				WIDGET_ADAPTABLE = TINY_CUSTOM_BEGIN << 0,
				WIDGET_VERTICAL = TINY_CUSTOM_BEGIN << 1,
				WIDGET_REQUIRE_BATCH = TINY_CUSTOM_BEGIN << 2,
				WIDGET_BATCHING = TINY_CUSTOM_BEGIN << 3,
				WIDGET_BATCHED = TINY_CUSTOM_BEGIN << 4,
				WIDGET_DISPLAY = TINY_CUSTOM_BEGIN << 5, 
				WIDGET_CUSTOM_BEGIN = TINY_CUSTOM_BEGIN << 6
			};

			// event slots
			struct Events {
				EventMouseClick eventMouseClick;
				EventMouseMove eventMouseMove;
				MessageShow eventShow;
				MessageHighlight eventHighlight;
				MessageTimer eventTimer;
				MessageDestroy eventDestroy;
			};

			class TextRangeOption : public IReflectObjectComplex {
			public:
				TextRangeOption();
				ZFloat4 color;
				bool reverseColor;
				virtual TObject<IReflect>& operator () (IReflect& reflect);
			};

			class TextCursorOption : public IReflectObjectComplex {
			public:
				TextCursorOption();
				ZFloat4 color;
				bool reverseColor;
				String ch;

				virtual TObject<IReflect>& operator () (IReflect& reflect);
			};

		public:
			Widget(Widget* parent = nullptr);
			virtual ~Widget();
			void AppendChild(Widget* w, bool head);
			void RemoveChild(Widget* w);
			void ClearChildren();
			void SetUpdateMark();
			void UpdateLayout(bool forceUpdate);
			TShared<TextControl> GetTextControl(bool createOnEmpty);
			void SetTexture(TShared<NsSnowyStream::TextureResource> texture, const ZFloat4& texCoord, const ZFloat4& percent);
			TObject<IReflect>& operator () (IReflect& reflect);

		public:
			// relationships
			Widget* parent;
			Widget* firstChild;
			Widget* nextSibling;

			// layouts
			struct Layout {
				Layout();
				ZInt2 scrollSize;
				ZInt2 scrollOffset;
				ZInt2Pair padding;
				ZInt2Pair margin;
				ZInt2Pair size;
				int32_t weight;
				int32_t count;
				uint32_t start;
				uint32_t remained;

				ZInt2Pair rect;
				ZInt2Pair clippedRect;
				ZFloat4 texCoordRect;
				ZFloat4 percentage;
				ZFloat4 color;

				uint64_t updateTimeStamp;
			} layout;
			TShared<NsSnowyStream::TextureResource> mainTexture;

		protected:
			TShared<TextControl> textControl;
		};
	}
}


#endif // __WIDGET_H__
