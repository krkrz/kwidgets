#define NOMINMAX
#include <Windows.h>

#include "ncbind.hpp"


class SystemSupport
{
public:
	static void sleep(tjs_int msec) {
		::Sleep(msec);
	}

	static tTJSString getKeyNameText(tjs_uint keyCode, tjs_uint shift)
	{
		ttstr result;

		if (keyCode == VK_PAUSE) {
			result = L"Pause";
		} else {
			wchar_t keyStr[256];
			tjs_uint scanCode;
			scanCode = MapVirtualKey(keyCode, 0);
			if ((33 <= keyCode && keyCode <= 46)
				|| (91 <= keyCode && keyCode <= 93)
				|| keyCode == 108
				|| keyCode == 111
				|| keyCode == 144
				|| keyCode == 163
				|| keyCode == 165)
				scanCode |= 0x100;
			if (scanCode) {
				if (GetKeyNameText(scanCode << 16, keyStr, 256)) {
					result = keyStr;
				}
			}
		}
		if (! result.IsEmpty()) {
			if (shift & 2) result = TJS_W("Alt+") + result;
			if (shift & 4) result = TJS_W("Ctrl+") + result;
			if (shift & 1) result = TJS_W("Shift+") + result;
		}
		return result;
	}
};

NCB_ATTACH_CLASS(SystemSupport, System)
{
	NCB_METHOD(sleep);
	NCB_METHOD(getKeyNameText);
};
