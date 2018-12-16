// ZFrameFreeglut.h -- App frame based on FreeGLUT
// By PaintDream (paintdream@paintdream.com)
// 2014-12-1
//

#ifndef __ZFRAMEFREEGLUT_H__
#define __ZFRAMEFREEGLUT_H__

#include "../../../Core/PaintsNow.h"
#include "../../../Core/Interface/IFrame.h"
#include <cstdlib>

namespace PaintsNow {
	class ZFrameFreeglut final : public IFrame {
	public:
		ZFrameFreeglut(const ZInt2& size = ZInt2(1600, 900), Callback* callback = nullptr);
		virtual void SetCallback(Callback* callback);
		virtual const ZInt2& GetWindowSize() const;
		virtual void SetWindowSize(const ZInt2& size);
		virtual void SetWindowTitle(const String& title);

		virtual bool IsRendering() const;
		virtual void OnMouse(const EventMouse& mouse);
		virtual void OnKeyboard(const EventKeyboard& keyboard);
		virtual void OnRender();
		virtual void OnWindowSize(const ZInt2& newSize);
		virtual void EnterMainLoop();
		virtual void ExitMainLoop();
		virtual void ShowCursor(CURSOR cursor);
		virtual void WarpCursor(const ZInt2& position);

	private:
		Callback* callback;
		ZInt2 windowSize;
		bool isRendering;
		bool mainLoopStarted;
	};
}


#endif // __ZFRAMEFREEGLUT_H__