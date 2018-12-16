// LunaWidget.h -- UI module
// By PaintDream (paintdream@paintdream.com)
// 2014-12-31
//

#ifndef __LUNAWIDGET_H__
#define __LUNAWIDGET_H__

#define D_SCL_SECURE_NO_WARNINGS

#include "../../Core/Interface/IScript.h"
#include "../../Core/Common/ZBatchRanger.h"
#include "../SnowyStream/SnowyStream.h"
#include "../SnowyStream/Resource/Pipelines/WidgetPipeline.h"
#include "../SnowyStream/Resource/MeshResource.h"
#include "../SnowyStream/Resource/TextureResource.h"
#include "Widget.h"
#include <set>

namespace PaintsNow {
	namespace NsLunaWidget {
		class LunaWidget : public IScript::Library {
		public:
			LunaWidget(IThread& thread, IRender& render, NsSnowyStream::SnowyStream& snowyStream);
			virtual ~LunaWidget();

			// static int main(int argc, char* argv[]);
			virtual void OnRender();
			virtual TObject<IReflect>& operator () (IReflect& reflect);
			virtual bool Initialize() override;
			virtual void Uninitialize() override;

		public:
			void RequestGetFrameRate(IScript::Request& request);
			void RequestNotifyMouseEvent(IScript::Request& request, bool down, bool left, bool move, bool wheel, int x, int y);
			void RequestNotifyKeyboardEvent(IScript::Request& request, int64_t key);
			void RequestCreateWidget(IScript::Request& request);
			void RequestSetRootWidget(IScript::Request& request, IScript::Delegate<Widget> widget);
			void RequestClearWidgetChildren(IScript::Request& request, IScript::Delegate<Widget> widget);
			void RequestSetWidgetParent(IScript::Request& request, IScript::Delegate<Widget> child, IScript::Delegate<Widget> parent, bool head);
			void RequestGetWidgetScrollSize(IScript::Request& request, IScript::Delegate<Widget> widget);
			void RequestGetWidgetScrollOffset(IScript::Request& request, IScript::Delegate<Widget> widget);
			void RequestSetWidgetScrollOffset(IScript::Request& request, IScript::Delegate<Widget> widget, ZInt2& position);
			void RequestSetWidgetFont(IScript::Request& request, IScript::Delegate<Widget> widget, String& font, int64_t fontSize, float reinforce);
			void RequestSetWidgetIndexRange(IScript::Request& request, IScript::Delegate<Widget> widget, int start, int count);
			void RequestSetWidgetFloat(IScript::Request& request, IScript::Delegate<Widget> widget, bool f);
			void RequestGetWidgetFloat(IScript::Request& request, IScript::Delegate<Widget> widget);
			void RequestGetWidgetText(IScript::Request& request, IScript::Delegate<Widget> widget);
			void RequestSetWidgetText(IScript::Request& request, IScript::Delegate<Widget> window, String& text);
			void RequestLocateWidgetText(IScript::Request& request, IScript::Delegate<Widget> window, ZInt2& offset, bool isRowCol);
			void RequestSetWidgetTextRangeOption(IScript::Request& request, IScript::Delegate<Widget> widget, ZInt2& range, Widget::TextRangeOption& rangeOption);
			void RequestSetWidgetTextCursorOption(IScript::Request& request, IScript::Delegate<Widget> widget, int pos, Widget::TextCursorOption& cursorOption);
			void RequestSetWidgetFitContent(IScript::Request& request, IScript::Delegate<Widget> window, bool fitContent);
			void RequestGetWidgetFitContent(IScript::Request& request, IScript::Delegate<Widget> window);
			void RequestSetWidgetImage(IScript::Request& request, IScript::Delegate<Widget> widget, String& image, ZFloat4& texCoords, ZFloat4& percent);
			void RequestSetWidgetBkColor(IScript::Request& request, IScript::Delegate<Widget> window, ZFloat4& color);
			void RequestGetWidgetBkColor(IScript::Request& request, IScript::Delegate<Widget> window);
			void RequestSetWidgetLayout(IScript::Request& request, IScript::Delegate<Widget> window, String& layout);
			void RequestSetWidgetWeight(IScript::Request& request, IScript::Delegate<Widget> window, int64_t weight);
			void RequestGetWidgetWeight(IScript::Request& request, IScript::Delegate<Widget> widget);
			void RequestSetWidgetPasswordChar(IScript::Request& request, IScript::Delegate<Widget> window, String& pwd);
			void RequestShowWidget(IScript::Request& request, IScript::Delegate<Widget> window, bool show);
			void RequestIsWidgetShow(IScript::Request& request, IScript::Delegate<Widget> window);
			void RequestSetWidgetRect(IScript::Request& request, IScript::Delegate<Widget> window, ZInt4& rect);
			void RequestGetWidgetRect(IScript::Request& request, IScript::Delegate<Widget> window);
			void RequestGetWidgetClippedRect(IScript::Request& request, IScript::Delegate<Widget> window);
			void RequestSetWidgetSize(IScript::Request& request, IScript::Delegate<Widget> window, ZInt4& size);
			void RequestGetWidgetSize(IScript::Request& request, IScript::Delegate<Widget> window);
			void RequestSetWidgetPadding(IScript::Request& request, IScript::Delegate<Widget> window, ZInt4& padding);
			void RequestGetWidgetPadding(IScript::Request& request, IScript::Delegate<Widget> window);
			void RequestSetWidgetMargin(IScript::Request& request, IScript::Delegate<Widget> window, ZInt4& margin);
			void RequestGetWidgetMargin(IScript::Request& request, IScript::Delegate<Widget> window);

		protected:
			void RenderWidget(Widget* widget, const ZInt2& scroll, const ZInt2Pair& clip, const ZInt2& size);
			void OnInvokeRender(IRender& render, ZPipeline& pipeline);
			void OnInitializeGraphics(VRender& render);

		protected:
			class WidgetBatcher : public SharedTiny, public ZBatchRanger {
			public:
				WidgetBatcher(ZPipeline& pipeline) : ZBatchRanger(pipeline) {}
			};

		protected:
			NsSnowyStream::SnowyStream& snowyStream;
			NsSnowyStream::WidgetPipeline widgetPipeline;
			NsSnowyStream::WidgetPipeline batchedWidgetPipeline;
			TShared<WidgetBatcher> widgetBatcher;

			TShared<NsSnowyStream::MeshResource> stdSquareMeshResource;
			TShared<NsSnowyStream::TextureResource> stdErrorTextureResource;
			TShared<Widget> rootWidget;
			uint64_t timeStamp;
			float fps;
		};
	}
}


#endif // __LUNAWIDGET_H__