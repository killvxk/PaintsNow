#include "LunaWidget.h"
#include "Widget.h"
#include "../../Core/Template/TFactory.h"
#include "../../Core/Interface/ITimer.h"

#include <string>
#include <map>

using namespace PaintsNow;
using namespace PaintsNow::NsLunaWidget;
using namespace PaintsNow::NsSnowyStream;

LunaWidget::LunaWidget(IThread& thread, IRender& render, SnowyStream& s) : Library(thread), snowyStream(s), timeStamp(0), fps(0) {
	Creatable<Widget::TextCursorOption>::Init();
	Creatable<Widget::TextRangeOption>::Init();

	// manually specified instanced variables
	// batch widgets as most as possible
	batchedWidgetPipeline.widgetTransform.inputPositionRect.SetType(IShaderVariableBase::INPUT_INSTANCED_BUFFER);
	batchedWidgetPipeline.widgetShading.inputTexCoordMark.SetType(IShaderVariableBase::INPUT_INSTANCED_BUFFER);
	batchedWidgetPipeline.widgetShading.inputTexCoordRect.SetType(IShaderVariableBase::INPUT_INSTANCED_BUFFER);
	batchedWidgetPipeline.widgetShading.inputTexCoordScale.SetType(IShaderVariableBase::INPUT_INSTANCED_BUFFER);
	batchedWidgetPipeline.widgetShading.tintColor.SetType(IShaderVariableBase::INPUT_INSTANCED_BUFFER);

	// install low-level render hook for batching
	widgetPipeline.eventFireRender += Wrap(this, &LunaWidget::OnInvokeRender);
}

void LunaWidget::OnInvokeRender(IRender& render, ZPipeline& pipeline) {
	assert(&pipeline == &widgetPipeline);
}

void LunaWidget::OnInitializeGraphics(VRender& r) {
	IRender& render = static_cast<IRender&>(r);
	widgetPipeline.Compile(render);
	batchedWidgetPipeline.Compile(render);
	widgetBatcher = TShared<WidgetBatcher>::Make(widgetPipeline);
}

bool LunaWidget::Initialize() {
	IRender& render = snowyStream.GetInterfaces().render;
	render.QueueRenderThreadRoutine(Wrap(this, &LunaWidget::OnInitializeGraphics));

	stdSquareMeshResource.Reset(snowyStream.CreateReflectedResource(UniqueType<MeshResource>(), "[Runtime]/MeshResource/StandardSquare"));
	stdErrorTextureResource.Reset(snowyStream.CreateReflectedResource(UniqueType<TextureResource>(), "[Runtime]/TextureResource/Missing"));

	return !!(stdSquareMeshResource) && !!(stdErrorTextureResource);
}

void LunaWidget::Uninitialize() {
	rootWidget.Release();
	stdSquareMeshResource.Release();
	stdErrorTextureResource.Release();

	// cleanup pipeline

	IRender& render = snowyStream.GetInterfaces().render;
	// batchNode.Cleanup(render);
	widgetPipeline.Cleanup(render);
	batchedWidgetPipeline.Cleanup(render);
}

LunaWidget::~LunaWidget() {
	
}

inline ZFloat4 Percentage(const ZFloat4& input, const ZFloat4& percentage) {
	ZFloat2 diff(input.z() - input.x(), input.w() - input.y());
	ZFloat2 p(input.x(), input.y());
	ZFloat2 start = p + diff * ZFloat2(percentage.x(), percentage.y());
	ZFloat2 end = p + diff * ZFloat2(percentage.z(), percentage.w());
	return ZFloat4(start.x(), start.y(), end.x(), end.y());
}

void LunaWidget::RenderWidget(Widget* first, const ZInt2& scroll, const ZInt2Pair& clip, const ZInt2& totalSize) {
	for (Widget* widget = first; widget != nullptr; widget = widget->nextSibling) {
		if (!(widget->Flag() & Widget::WIDGET_DISPLAY)) continue;
		widget->UpdateLayout(false);

		// constant attributes
		IRender& render = snowyStream.GetInterfaces().render;
		const ZFloat4& percentage = widget->layout.percentage;
		const ZFloat4& texCoordRect = widget->layout.texCoordRect;
		const ZFloat4& color = widget->layout.color;
		IRender::Texture* texture = widget->mainTexture ? widget->mainTexture->GetTexture() : nullptr;

		// computed attributes	
		ZInt2 newScroll = scroll + widget->layout.scrollOffset;
		ZInt2Pair orgRect = ZInt2Pair(widget->layout.rect.first + newScroll, widget->layout.rect.second + newScroll);
		ZInt2Pair rect = orgRect;
		Clip(rect, clip);

		ZInt2Pair region = ZInt2Pair(widget->layout.rect.first + newScroll, widget->layout.rect.second + scroll + widget->layout.scrollSize);
		Clip(region, clip);
		widget->layout.clippedRect = region;

		// get percentage of clipped rect
		ZInt2 orgSize(Max(orgRect.second.x() - orgRect.first.x(), 1), Max(orgRect.second.y() - orgRect.first.y(), 1));
		ZFloat4 clipPercentage = ZFloat4(
			Max((float)(rect.first.x() - orgRect.first.x()) / orgSize.x(), percentage.x()),
			Max((float)(rect.first.y() - orgRect.first.y()) / orgSize.y(), percentage.y()),
			Min((float)(rect.second.x() - orgRect.first.x()) / orgSize.x(), percentage.z()),
			Min((float)(rect.second.y() - orgRect.first.y()) / orgSize.y(), percentage.w())
		);

		// skip invisible widgets ...
		if (clipPercentage.z() <= clipPercentage.x() || clipPercentage.w() <= clipPercentage.y()) {
			continue;
		}

		// write shader values ...
		if (texture != nullptr) {
			ZFloat4 stdPositionRect = ZFloat4(
				(float)rect.first.x() / totalSize.x(),
				1 - (float)rect.second.y() / totalSize.y(),
				(float)rect.second.x() / totalSize.x(),
				1 - (float)rect.first.y() / totalSize.y()
			);

			ZInt2 orgSize(orgRect.second.x() - orgRect.first.x(), orgRect.second.y() - orgRect.first.y());
			ZFloat4 texCoordMark(
				(float)widget->layout.padding.first.x() / orgSize.x(),
				(float)widget->layout.padding.first.y() / orgSize.y(),
				1.0f - (float)widget->layout.padding.second.x() / orgSize.x(),
				1.0f - (float)widget->layout.padding.second.y() / orgSize.y()
			);

			size_t texWidth, texHeight;
			render.GetTextureSize(texture, 0, texWidth, texHeight);
			float texX = texWidth * (texCoordRect.z() - texCoordRect.x());
			float texY = texHeight * (texCoordRect.w() - texCoordRect.y());
			// orgSize.x() / texX * (texCoordMark.x() + 1 - texCoordMark.z()) + ? * (texCoordMark.z() - texCoordMark.x()) = 1
			float sx = orgSize.x() / texX, sy = orgSize.y() / texY;
			float ex = (1 - sx * (texCoordMark.x() + 1 - texCoordMark.z())) / (texCoordMark.z() - texCoordMark.x());
			float ey = (1 - sy * (texCoordMark.y() + 1 - texCoordMark.w())) / (texCoordMark.w() - texCoordMark.y());
			ZFloat4 texCoordScale(sx, sy, ex, ey);

			widgetPipeline.widgetTransform.inputPositionRect = Percentage(stdPositionRect, percentage) * 2.0f - 1.0f;
			widgetPipeline.widgetShading.inputTexCoordMark = Percentage(texCoordMark, percentage);
			widgetPipeline.widgetShading.inputTexCoordScale = texCoordScale;
			widgetPipeline.widgetShading.inputTexCoordRect = Percentage(texCoordRect, percentage);
			widgetPipeline.widgetShading.tintColor = color;
			widgetPipeline.widgetShading.mainTexture = texture;
			widgetPipeline.FireRender(render);
		}

		TShared<TextControl> textControl = widget->GetTextControl(false);
		if (textControl) {
			textControl->Scroll(rect.first - orgRect.first);
			rect.first += widget->layout.padding.first;
			rect.second -= widget->layout.padding.second;
			textControl->FireRender(render, widgetPipeline, rect, totalSize, ZInt2(0, 0));
		}

		// process remaining
		RenderWidget(widget->firstChild, newScroll, region, totalSize);
	}
}

void LunaWidget::OnRender() {
	uint64_t now = ITimer::GetSystemClock();
	int64_t diff = now - timeStamp;
	if (diff != 0) {
		fps = 1000.0f / diff;
	}

	IRender& render = snowyStream.GetInterfaces().render;

	// iterate widgets
	// Bind buffers
	if (stdSquareMeshResource) {
		IRender::Target* renderTarget = nullptr; // to screen
		IRender::Buffer* positionBuffer = stdSquareMeshResource->bufferCollection.positionBuffer;
		IRender::Buffer* indexBuffer = stdSquareMeshResource->bufferCollection.indexBuffer;
		assert(positionBuffer != nullptr && indexBuffer != nullptr);

		size_t width, height;
		render.GetTargetSize(renderTarget, width, height);
		if (widgetPipeline.IsCompiled()) {
			IRender::Program* program = widgetPipeline.GetProgram();
			render.SetCullFace(program, true);
			render.SetBlend(program, true, IRender::SRC_ALPHA, IRender::ONE);
			render.SetDepthTest(program, false);

			widgetPipeline.Attach(render, renderTarget);
			ZFloat4 texCoordMark(0.5f, 0.5f, 0.5f, 0.5f);
			ZFloat4 texCoordScale(1, 1, 1, 1);
			widgetPipeline.BindIndexBuffer(render, indexBuffer, 0, 0);
			widgetPipeline.widgetTransform.unitPositionTexCoord = positionBuffer;
			widgetPipeline.widgetTransform.inputPositionRect = ZFloat4(-1.0f, -1.0f, 1.0f, 1.0f);
			widgetPipeline.widgetShading.tintColor = ZFloat4(1.0f, 1.0f, 1.0f, 1.0f);
			widgetPipeline.widgetShading.mainTexture = stdErrorTextureResource->instance;
			widgetPipeline.widgetShading.inputTexCoordMark = texCoordMark;
			widgetPipeline.widgetShading.inputTexCoordScale = texCoordScale;
			widgetPipeline.widgetShading.inputTexCoordRect = ZFloat4(0.0f, 0.0f, 1.0f, 1.0f);
			// test render!
			// widgetPipeline.FireRender(render);

			RenderWidget(rootWidget(), ZInt2(0, 0), ZInt2Pair(ZInt2(0, 0), ZInt2((int)width, (int)height)), ZInt2((int)width, (int)height));
			widgetPipeline.Detach(render, renderTarget);
		}
	}

	timeStamp = now;
}

TObject<IReflect>& LunaWidget::operator ()(IReflect& reflect) {
	ReflectClass(LunaWidget)[Interface(IScript::Library)];
	if (reflect.IsReflectProperty()) {
		ReflectProperty(snowyStream);
	}

	if (reflect.IsReflectMethod()) {
		// Utils
		ReflectMethod(RequestGetFrameRate)[ScriptMethod = "GetFrameRate"];
		ReflectMethod(RequestNotifyMouseEvent)[ScriptMethod = "NotifyMouseEvent"];
		ReflectMethod(RequestNotifyKeyboardEvent)[ScriptMethod = "NotifyKeyboardEvent"];

		// Widget managements
		ReflectMethod(RequestCreateWidget)[ScriptMethod = "CreateWidget"];
		ReflectMethod(RequestSetRootWidget)[ScriptMethod = "SetRootWidget"];
		ReflectMethod(RequestClearWidgetChildren)[ScriptMethod = "ClearWidgetChildren"];
		ReflectMethod(RequestSetWidgetParent)[ScriptMethod = "SetWidgetParent"];

		// Widget attributes
		ReflectMethod(RequestGetWidgetScrollSize)[ScriptMethod = "GetWidgetScrollSize"];
		ReflectMethod(RequestGetWidgetScrollOffset)[ScriptMethod = "GetWidgetScrollOffset"];
		ReflectMethod(RequestSetWidgetScrollOffset)[ScriptMethod = "SetWidgetScrollOffset"];
		ReflectMethod(RequestSetWidgetFont)[ScriptMethod = "SetWidgetFont"];
		ReflectMethod(RequestSetWidgetIndexRange)[ScriptMethod = "SetWidgetIndexRange"];
		ReflectMethod(RequestSetWidgetFloat)[ScriptMethod = "SetWidgetFloat"];
		ReflectMethod(RequestGetWidgetFloat)[ScriptMethod = "GetWidgetFloat"];
		ReflectMethod(RequestGetWidgetText)[ScriptMethod = "GetWidgetText"];
		ReflectMethod(RequestSetWidgetText)[ScriptMethod = "SetWidgetText"];
		ReflectMethod(RequestLocateWidgetText)[ScriptMethod = "LocateWidgetText"];
		ReflectMethod(RequestSetWidgetTextRangeOption)[ScriptMethod = "SetWidgetTextRangeOption"];
		ReflectMethod(RequestSetWidgetTextCursorOption)[ScriptMethod = "SetWidgetTextCursorOption"];
		ReflectMethod(RequestSetWidgetBkColor)[ScriptMethod = "SetWidgetBkColor"];
		ReflectMethod(RequestGetWidgetBkColor)[ScriptMethod = "GetWidgetBkColor"];
		ReflectMethod(RequestSetWidgetImage)[ScriptMethod = "SetWidgetImage"];
		ReflectMethod(RequestSetWidgetLayout)[ScriptMethod = "SetWidgetLayout"];
		ReflectMethod(RequestSetWidgetWeight)[ScriptMethod = "SetWidgetWeight"];
		ReflectMethod(RequestGetWidgetWeight)[ScriptMethod = "GetWidgetWeight"];
		ReflectMethod(RequestSetWidgetPasswordChar)[ScriptMethod = "SetWidgetPasswordChar"];
		ReflectMethod(RequestShowWidget)[ScriptMethod = "ShowWidget"];
		ReflectMethod(RequestIsWidgetShow)[ScriptMethod = "IsWidgetShow"];
		ReflectMethod(RequestSetWidgetRect)[ScriptMethod = "SetWidgetRect"];
		ReflectMethod(RequestGetWidgetRect)[ScriptMethod = "GetWidgetRect"];
		ReflectMethod(RequestGetWidgetClippedRect)[ScriptMethod = "GetWidgetClippedRect"];
		ReflectMethod(RequestSetWidgetSize)[ScriptMethod = "SetWidgetSize"];
		ReflectMethod(RequestGetWidgetSize)[ScriptMethod = "GetWidgetSize"];
		ReflectMethod(RequestSetWidgetFitContent)[ScriptMethod = "SetWidgetFitContent"];
		ReflectMethod(RequestGetWidgetFitContent)[ScriptMethod = "GetWidgetFitContent"];
		ReflectMethod(RequestSetWidgetPadding)[ScriptMethod = "SetWidgetPadding"];
		ReflectMethod(RequestGetWidgetPadding)[ScriptMethod = "GetWidgetPadding"];
		ReflectMethod(RequestSetWidgetMargin)[ScriptMethod = "SetWidgetMargin"];
		ReflectMethod(RequestGetWidgetMargin)[ScriptMethod = "GetWidgetMargin"];
	}

	return *this;
}

void LunaWidget::RequestGetFrameRate(IScript::Request& request) {
	METHOD_ENTRY(LunaWidget::RequestGetFrameRate);
	CHECK_REFERENCES_NONE();
	request.DoLock();
	request << fps;
	request.UnLock();
}

void LunaWidget::RequestGetWidgetFloat(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetFloat);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);
	
	bool floated = !(widget->Flag() & Tiny::TINY_PINNED);
	request.DoLock();
	request << floated;
	request.UnLock();
}

void LunaWidget::RequestSetWidgetFloat(IScript::Request& request, IScript::Delegate<Widget> widget, bool f) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetFloat);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	if (f) {
		widget->Flag() &= ~Tiny::TINY_PINNED;
	} else {
		widget->Flag() |= Tiny::TINY_PINNED;
	}

	widget->SetUpdateMark();
}

void LunaWidget::RequestSetWidgetPasswordChar(IScript::Request& request, IScript::Delegate<Widget> widget, String& pwd) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetPasswordChar);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	widget->GetTextControl(true)->passwordChar = pwd[0];
	widget->SetUpdateMark();
}

void LunaWidget::RequestSetWidgetTextRangeOption(IScript::Request& request, IScript::Delegate<Widget> widget, ZInt2& range, Widget::TextRangeOption& option) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetTextRangeOption);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	TShared<TextControl> textControl = widget->GetTextControl(true);
	Widget* w = widget.Get();
	if (range.x() > range.y()) {
		std::swap(range.x(), range.y());
	}

	textControl->selectRange = range;
	textControl->selectColor = option.color;
	textControl->selectRevColor = option.reverseColor;
	textControl->Flag() |= Tiny::TINY_MODIFIED;
	w->SetUpdateMark();
}

void LunaWidget::RequestSetWidgetTextCursorOption(IScript::Request& request, IScript::Delegate<Widget> widget, int pos, Widget::TextCursorOption& option) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetTextCursorOption);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);
	
	TShared<TextControl> textControl = widget->GetTextControl(true);
	textControl->cursorPos = pos;
	textControl->cursorChar = option.ch[0];
	textControl->cursorColor = option.color;
	textControl->cursorRevColor = option.reverseColor;
	textControl->Flag() |= Tiny::TINY_MODIFIED;
	widget->SetUpdateMark();
}

void LunaWidget::RequestSetWidgetIndexRange(IScript::Request& request, IScript::Delegate<Widget> widget, int start, int count) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetIndexRange);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	widget->layout.start = start;
	widget->layout.count = count;
	widget->SetUpdateMark();
}

void LunaWidget::RequestGetWidgetClippedRect(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetClippedRect);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	request.DoLock();
	request << ZInt4(widget->layout.clippedRect);
	request.UnLock();
}

void LunaWidget::RequestGetWidgetRect(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetRect);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	request.DoLock();
	request << ZInt4(widget->layout.rect);
	request.UnLock();
}

void LunaWidget::RequestSetWidgetLayout(IScript::Request& request, IScript::Delegate<Widget> widget, String& layout) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetLayout);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	if (layout == "horizontal") {
		widget->Flag() &= ~Widget::WIDGET_VERTICAL;
	} else {
		widget->Flag() |= Widget::WIDGET_VERTICAL;
	}

	widget->SetUpdateMark();
}

void LunaWidget::RequestSetWidgetImage(IScript::Request& request, IScript::Delegate<Widget> widget, String& image, ZFloat4& texCoords, ZFloat4& percent) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetImage);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	TextureResource* textureResource = snowyStream.CreateReflectedResource(UniqueType<TextureResource>(), image);
	if (textureResource != nullptr) {
		widget->SetTexture(textureResource, texCoords, percent);
		textureResource->ReleaseObject();
	} else {
		request.Error(String("Unable to load texture: ") + image);
	}

}

void LunaWidget::RequestSetWidgetBkColor(IScript::Request& request, IScript::Delegate<Widget> widget, ZFloat4& color) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetBkColor);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	widget->layout.color = color;
	widget->SetUpdateMark();
}

void LunaWidget::RequestGetWidgetBkColor(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetBkColor);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	request.DoLock();
	request << widget->layout.color;
	request.UnLock();
}

void LunaWidget::RequestSetWidgetFitContent(IScript::Request& request, IScript::Delegate<Widget> widget, bool fitContent) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetFitContent);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	widget->Flag() |= Widget::WIDGET_ADAPTABLE;
	widget->SetUpdateMark();
}

void LunaWidget::RequestGetWidgetFitContent(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetFitContent);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);
	
	request.DoLock();
	request << !!(widget->Flag() & Widget::WIDGET_ADAPTABLE);
	request.UnLock();
}

void LunaWidget::RequestNotifyMouseEvent(IScript::Request& request, bool down, bool left, bool move, bool wheel, int x, int y) {
	METHOD_ENTRY(LunaWidget::RequestNotifyMouseEvent);
	CHECK_REFERENCES_NONE();

	// no operations by now
}

void LunaWidget::RequestNotifyKeyboardEvent(IScript::Request& request, int64_t key) {
	METHOD_ENTRY(LunaWidget::RequestNotifyKeyboardEvent);
	CHECK_REFERENCES_NONE();

	// no operations by now	
}

void LunaWidget::RequestCreateWidget(IScript::Request& request) {
	METHOD_ENTRY(LunaWidget::RequestCreateWidget);
	CHECK_REFERENCES_NONE();
	
	TShared<Widget> widget = TShared<Widget>::Make();
	request.DoLock();
	request << delegate(widget());
	request.UnLock();
}

void LunaWidget::RequestSetRootWidget(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestSetRootWidget);
	CHECK_REFERENCES_NONE();

	Widget* w = widget.Get();
	if (rootWidget() != w) {
		rootWidget = w;
		widget->SetUpdateMark();
	}
}


void LunaWidget::RequestClearWidgetChildren(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestClearWidgetChildren);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	widget->ClearChildren();
}

void LunaWidget::RequestSetWidgetParent(IScript::Request& request, IScript::Delegate<Widget> child, IScript::Delegate<Widget> parent, bool head) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetParent);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(child);

	Widget* c = child.Get();
	Widget* p = parent.Get();
	Widget* cp = c->parent;

	if (p != cp) { // clear
		if (cp != nullptr) {
			cp->RemoveChild(c);
		}

		if (p != nullptr) {
			p->AppendChild(c, head);
		}

		c->SetUpdateMark();
		p->SetUpdateMark();
	}
}


void LunaWidget::RequestSetWidgetFont(IScript::Request& request, IScript::Delegate<Widget> widget, String& font, int64_t fontSize, float reinforce) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetFont);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	// Get font
	FontResource* fontResource = snowyStream.CreateReflectedResource(UniqueType<FontResource>(), font);
	if (fontResource != nullptr) {
		TShared<TextControl> textControl = widget->GetTextControl(true);
		textControl->mainFont = fontResource;
		textControl->fontSize = (uint32_t)fontSize;
		fontResource->ReleaseObject();
		widget->SetUpdateMark();
	}
}

void LunaWidget::RequestGetWidgetScrollSize(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetScrollSize);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);
	
	request.DoLock();
	request << widget->layout.scrollSize;
	request.UnLock();
}

void LunaWidget::RequestGetWidgetScrollOffset(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetScrollOffset);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	request.DoLock();
	request << widget->layout.scrollOffset;
	request.UnLock();
}

void LunaWidget::RequestSetWidgetScrollOffset(IScript::Request& request, IScript::Delegate<Widget> widget, ZInt2& position) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetScrollOffset);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	widget->layout.scrollOffset = position;
	widget->SetUpdateMark();
}

void LunaWidget::RequestShowWidget(IScript::Request& request, IScript::Delegate<Widget> widget, bool show) {
	METHOD_ENTRY(LunaWidget::RequestShowWidget);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	if (show) {
		widget->Flag() |= Widget::WIDGET_DISPLAY;
	} else {
		widget->Flag() &= ~Widget::WIDGET_DISPLAY;
	}

	for (Widget* w = widget.Get(); w != nullptr; w = w->parent) {
		w->SetUpdateMark();
	}
}

void LunaWidget::RequestGetWidgetText(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetText);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	TShared<TextControl> textControl = widget->GetTextControl(false);
	request.DoLock();
	request << (textControl ? textControl->text : "");
	request.UnLock();
}

void LunaWidget::RequestSetWidgetText(IScript::Request& request, IScript::Delegate<Widget> widget, String& text) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetText);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	TShared<TextControl> textControl = widget->GetTextControl(true);
	textControl->SetText(text);
	textControl->Flag() |= Tiny::TINY_MODIFIED;
	widget->SetUpdateMark();
}

void LunaWidget::RequestLocateWidgetText(IScript::Request& request, IScript::Delegate<Widget> widget, ZInt2& offset, bool isRowCol) {
	METHOD_ENTRY(LunaWidget::RequestLocateWidgetText);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	TShared<TextControl> textControl = widget->GetTextControl(true);
	ZInt2 rowCol;
	int loc = textControl->Locate(rowCol, offset - widget->layout.clippedRect.first, isRowCol);
	request.DoLock();
	request << ZInt3(rowCol.x(), rowCol.y(), loc);
	request.UnLock();
}

void LunaWidget::RequestSetWidgetRect(IScript::Request& request, IScript::Delegate<Widget> widget, ZInt4& rect) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetRect);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	// assert(rect.x() >= 0 && rect.y() >= 0 && rect.z() >= 0 && rect.w() >= 0);
	
	widget->layout.rect = ZInt2Pair(ZInt2(rect.x(), rect.y()), ZInt2(rect.z(), rect.w()));
	widget->SetUpdateMark();
}

void LunaWidget::RequestIsWidgetShow(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestIsWidgetShow);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	request.DoLock();
	request << !!(widget->Flag() & Widget::WIDGET_DISPLAY);
	request.UnLock();
}

void LunaWidget::RequestSetWidgetWeight(IScript::Request& request, IScript::Delegate<Widget> widget, int64_t weight) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetWeight);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);
	
	widget->layout.weight = (int32_t)weight;
	widget->SetUpdateMark();
}

void LunaWidget::RequestGetWidgetWeight(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetWeight);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	request.DoLock();
	request << widget->layout.weight;
	request.UnLock();
}

void LunaWidget::RequestGetWidgetSize(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetSize);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	request.DoLock();
	request << ZInt4(widget->layout.size);
	request.UnLock();
}

void LunaWidget::RequestSetWidgetSize(IScript::Request& request, IScript::Delegate<Widget> widget, ZInt4& size) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetSize);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	widget->layout.size = ZInt2Pair(ZInt2(size.x(), size.y()), ZInt2(size.z(), size.w()));
	widget->SetUpdateMark();
	if (widget->parent != nullptr) {
		widget->parent->SetUpdateMark();
	}
}

void LunaWidget::RequestSetWidgetPadding(IScript::Request& request, IScript::Delegate<Widget> widget, ZInt4& size) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetPadding);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);
	assert(size.x() >= 0 && size.y() >= 0 && size.z() >= 0 && size.w() >= 0);

	widget->layout.padding = ZInt2Pair(ZInt2(size.x(), size.y()), ZInt2(size.z(), size.w()));
	widget->SetUpdateMark();
}

void LunaWidget::RequestGetWidgetPadding(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestGetWidgetPadding);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	request.DoLock();
	request << ZInt4(widget->layout.padding);
	request.UnLock();
}

void LunaWidget::RequestSetWidgetMargin(IScript::Request& request, IScript::Delegate<Widget> widget, ZInt4& size) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetMargin);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);
	
	widget->layout.margin = ZInt2Pair(ZInt2(size.x(), size.y()), ZInt2(size.z(), size.w()));
	widget->SetUpdateMark();
}

void LunaWidget::RequestGetWidgetMargin(IScript::Request& request, IScript::Delegate<Widget> widget) {
	METHOD_ENTRY(LunaWidget::RequestSetWidgetMargin);
	CHECK_REFERENCES_NONE();
	CHECK_DELEGATE(widget);

	request.DoLock();
	request << ZInt4(widget->layout.margin);
	request.UnLock();
}
