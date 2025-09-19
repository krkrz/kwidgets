// -*- coding: utf-8 -*-

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <cstdio>

#include "tp_stub.h"
#include "ncbind.hpp"


#pragma comment(lib, "Dwmapi.lib")

static const int RESIZE_BORDER_WIDTH = 8;


class WindowSupport
{
private:
	iTJSDispatch2 *mObjthis;
	HWND mHwnd;
	int mReceiverRefCount;
	bool mBorderless;
	bool mBorderlessShadow;
	bool mBorderlessDrag;
	bool mBorderlessResize;
	RECT mCaptionRect;

public:
	WindowSupport(iTJSDispatch2 *objthis)
		: mObjthis(objthis)
		, mReceiverRefCount(0)
		, mBorderless(false)
		, mBorderlessShadow(true)
		, mBorderlessDrag(false)
		, mBorderlessResize(true)
	{
		mHwnd = (HWND)ncbPropAccessor(mObjthis, mObjthis).GetValue(L"HWND", ncbTypedefs::Tag<tjs_int64>());
		mCaptionRect = { 0, 0, 0, 0 };
	}

	~WindowSupport() {
		while (mReceiverRefCount > 0)
			releaseReceiver();
	}

	enum class Style : DWORD {
		windowed		 = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN,
		aero_borderless  = WS_POPUP			   | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
		basic_borderless = WS_POPUP			   | WS_THICKFRAME				| WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN
	};

	bool maximized(void) const {
		WINDOWPLACEMENT placement;
		if (!::GetWindowPlacement(mHwnd, &placement)) {
			return false;
		}

		return placement.showCmd == SW_MAXIMIZE;
	}

	void adjust_maximized_client_rect(RECT& rect) {
		if (! maximized()) {
			return;
		}

		auto monitor = ::MonitorFromWindow(mHwnd, MONITOR_DEFAULTTONULL);
		if (! monitor) {
			return;
		}

		MONITORINFO monitor_info{};
		monitor_info.cbSize = sizeof(monitor_info);
		if (!::GetMonitorInfoW(monitor, &monitor_info)) {
			return;
		}

		// when maximized, make the client area fill just the monitor (without task bar) rect,
		// not the whole window rect which extends beyond the monitor.
		rect = monitor_info.rcWork;
	}

	Style select_borderless_style() const {
		return composition_enabled() ? Style::aero_borderless : Style::basic_borderless;
	}

	bool composition_enabled() const {
		BOOL composition_enabled = FALSE;
		bool success = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
		return composition_enabled && success;
	}

	auto set_shadow(bool enabled) -> void {
		if (composition_enabled()) {
			static const MARGINS shadow_state[2]{ { 0,0,0,0 },{ 1,1,1,1 } };
			::DwmExtendFrameIntoClientArea(mHwnd, &shadow_state[enabled]);
		}
	}

	LRESULT hit_test(POINT cursor) const {
		// identify borders and corners to allow resizing the window.
		// Note: On Windows 10, windows behave differently and
		// allow resizing outside the visible window frame.
		// This implementation does not replicate that behavior.
		const POINT border{
			::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
			::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER)
		};
		RECT window;
		if (!::GetWindowRect(mHwnd, &window)) {
			return HTNOWHERE;
		}

		const auto inCaption = (window.left + mCaptionRect.left <= cursor.x && cursor.x < window.left + mCaptionRect.right
								&& window.top + mCaptionRect.top <= cursor.y && cursor.y < window.top + mCaptionRect.bottom);
		const auto drag = (inCaption || mBorderlessDrag) ? HTCAPTION : HTCLIENT;

		enum region_mask {
			client = 0b0000,
			left   = 0b0001,
			right  = 0b0010,
			top	   = 0b0100,
			bottom = 0b1000,
		};

		const auto result =
			left	* (cursor.x <  (window.left	  + border.x)) |
			right	* (cursor.x >= (window.right  - border.x)) |
			top		* (cursor.y <  (window.top	  + border.y)) |
			bottom	* (cursor.y >= (window.bottom - border.y));

		switch (result) {
		case left		   : return mBorderlessResize ? HTLEFT		  : drag;
		case right		   : return mBorderlessResize ? HTRIGHT		  : drag;
		case top		   : return mBorderlessResize ? HTTOP		  : drag;
		case bottom		   : return mBorderlessResize ? HTBOTTOM	  : drag;
		case top | left	   : return mBorderlessResize ? HTTOPLEFT	  : drag;
		case top | right   : return mBorderlessResize ? HTTOPRIGHT	  : drag;
		case bottom | left : return mBorderlessResize ? HTBOTTOMLEFT  : drag;
		case bottom | right: return mBorderlessResize ? HTBOTTOMRIGHT : drag;
		case client		   : return drag;
		default			   : return HTNOWHERE;
		}
	}

	void requireReceiver() {
		if (mReceiverRefCount++ == 0) {
			ncbPropAccessor window(mObjthis, mObjthis);
			window.FuncCall(0, L"registerMessageReceiver", NULL, NULL, wrmRegister, tTVInteger(&receiver), tTVInteger(mObjthis));
		}
	}

	void releaseReceiver() {
		if (--mReceiverRefCount == 0) {
			ncbPropAccessor window(mObjthis, mObjthis);
			window.FuncCall(0, L"registerMessageReceiver", NULL, NULL, wrmUnregister, tTVInteger(&receiver), tTVInteger(mObjthis));
		}
	}

	static bool __stdcall receiver(void *userdata, tTVPWindowMessage *mes) {
		auto obj = (iTJSDispatch2*)userdata;
		WindowSupport *inst = ncbInstanceAdaptor<WindowSupport>::GetNativeInstance(obj);
		if (inst != NULL)
			return inst->onMessage(mes);
		else
			return false;
	}

	bool onMessage(tTVPWindowMessage *mes) {
		switch (mes->Msg) {
		case WM_NCCALCSIZE: {
			if (mBorderless && mes->WParam == TRUE) {
				auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(mes->LParam);
				adjust_maximized_client_rect(params.rgrc[0]);
				mes->Result = 0;
				return true;
			}
			break;
		}

		case WM_NCHITTEST: {
			if (mBorderless) {
				mes->Result = hit_test(POINT{  GET_X_LPARAM(mes->LParam), GET_Y_LPARAM(mes->LParam) });
				return true;
			}
			return false;
		}

		case WM_NCACTIVATE: {
			if (! composition_enabled()) {
				mes->Result = 1;
				return true;
			}
			return false;
		}

		}

		return false;
	}

	void setBorderless(bool enabled) {
		if (mBorderless == enabled)
			return;

		mBorderless = enabled;

		if (mBorderless)
			requireReceiver();
		else
			releaseReceiver();

		Style new_style = (enabled) ? select_borderless_style() : Style::windowed;
		Style old_style = static_cast<Style>(::GetWindowLongPtrW(mHwnd, GWL_STYLE));

		if (new_style != old_style) {
			::SetWindowLongPtrW(mHwnd, GWL_STYLE, static_cast<LONG>(new_style));

			// when switching between borderless and windowed, restore appropriate shadow state
			set_shadow(mBorderlessShadow && (new_style != Style::windowed));

			// redraw frame
			if (::IsWindowVisible(mHwnd)) {
				::SetWindowPos(mHwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
				::ShowWindow(mHwnd, SW_SHOW);
			}
		}
	}

	bool getBorderless(void) const {
		return mBorderless;
	}

	void setBorderlessShadow(bool enabled) {
		if (mBorderless) {
			mBorderlessShadow = enabled;
			set_shadow(enabled);
		}
	}

	bool getBorderlessShadow(void) const {
		return mBorderlessShadow;
	}

	void setBorderlessDrag(bool state) {
		mBorderlessDrag = state;
	}

	bool getBorderlessDrag(void) const {
		return mBorderlessDrag;
	}

	void setBorderlessResize(bool state) {
		mBorderlessResize = state;
	}

	bool getBorderlessResize(void) const {
		return mBorderlessResize;
	}

	void pushClose() {
		PostMessage(mHwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
	}

	void setCaptionRect(tjs_int l, tjs_int t, tjs_int w, tjs_int h) {
		mCaptionRect = { l, t, l + w, t + h };
	}
};

NCB_GET_INSTANCE_HOOK(WindowSupport)
{
  NCB_INSTANCE_GETTER(objthis) { // objthis を iTJSDispatch2* 型の引数とする
    ClassT* obj = GetNativeInstance(objthis);	// ネイティブインスタンスポインタ取得
    if (!obj) {
      obj = new ClassT(objthis);				// ない場合は生成する
      SetNativeInstance(objthis, obj);		// objthis に obj をネイティブインスタンスとして登録する
    }
    return obj;
  }
};

NCB_ATTACH_CLASS_WITH_HOOK(WindowSupport, Window) {
	NCB_PROPERTY(borderless, getBorderless, setBorderless);
	NCB_PROPERTY(borderlessShadow, getBorderlessShadow, setBorderlessShadow);
	NCB_PROPERTY(borderlessDrag, getBorderlessDrag, setBorderlessDrag);
	NCB_PROPERTY(borderlessResize, getBorderlessResize, setBorderlessResize);
	NCB_METHOD(pushClose);
	NCB_METHOD(setCaptionRect);
};
