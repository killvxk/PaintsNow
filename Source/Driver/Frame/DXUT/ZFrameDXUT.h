// ZFrameDXUT.h -- App frame based on FreeGLUT
// By PaintDream (paintdream@paintdream.com)
// 2014-12-1
//

#ifndef __ZFRAMEDXUT_H__
#define __ZFRAMEDXUT_H__

#ifdef CMAKE_PAINTSNOW

#include "../../../Core/PaintsNow.h"
#include "../../../Core/Interface/IFrame.h"
#include "Core/DXUT.h"
#include "Core/DXUTcamera.h"
#include "Core/SDKmisc.h"
#include <cstdlib>

namespace PaintsNow {
	class ZFrameDXUT : public IFrame {
	public:
		ZFrameDXUT(const ZInt2& size = ZInt2(512, 512), Callback* callback = nullptr);
		virtual void SetCallback(Callback* callback);
		virtual const ZInt2& GetWindowSize() const;
		virtual void SetWindowSize(const ZInt2& size);
		virtual void SetWindowTitle(const String& title);

		virtual void OnMouse(const EventMouse& mouse);
		virtual void OnKeyboard(const EventKeyboard& keyboard);
		virtual void OnRender();
		virtual void OnWindowSize(const ZInt2& newSize);
		virtual void OnInitialize(void* param);
		virtual void EnterMainLoop();
		virtual void ExitMainLoop();
		virtual void FireRender();
		virtual void ShowCursor(CURSOR cursor);
		virtual void WarpCursor(const ZInt2& position);

	private:
		Callback* callback;
		String windowTitle;
		ZInt2 windowSize;
		HWND hWnd;
		bool mainLoopStarted;
	};
}

#endif

#endif // __ZFRAMEDXUT_H__