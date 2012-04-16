#include <windows.h>
#include "../ksupport/ncbind.hpp"

#ifdef _DEBUG
#define LOG(log) TVPAddLog(log)
#endif
#ifndef _DEBUG
#define LOG(log) {}
#endif

//	�v���p�e�B�̎擾
BOOL getValue(iTJSDispatch2* elm, tjs_char* key, tTJSVariant& value);

//	iTJSDispatch2����l�����o��
BOOL	getValue(iTJSDispatch2* elm, tjs_char* key, tTJSVariant& value)
{
	if(elm==NULL)
	{
		LOG(L"getValue faild. (elm == NULL)");
		return false;
	}
	tjs_error	isvalid	= elm->IsValid(0, key, NULL, elm);
	LOG(L"elm->IsValid = "+ttstr(isvalid));
	if(isvalid==TJS_S_FALSE)	// isvalid != TJS_S_TRUE �Ƃ���ƁA�������Ă����s���Ă��܂�
								// (Window.HWND ����낤�Ƃ���ƁATJS_E_NOTIMPL(-1002) ���Ԃ��Ă���)
	{
		LOG(L"getValue faild. (elm is invalid)");
		return false;
	}
	if(TJS_SUCCEEDED(elm->PropGet(0, key, NULL, &value, elm)))
		return true;
	else
	{
		LOG(L"getValue falid. (can't get property["+ttstr(key)+L"])");
		return false;
	}
}

class LayerExScreen
{
private:
	//	primaryLayer ��̍��W�����߂�
	static tjs_int getLayerAbsolutePos(iTJSDispatch2 * obj, tjs_char * prop)
	{
		tTJSVariant	value;
		tjs_int	tmp = 0;
		do
		{
			getValue(obj, prop, value);
			tmp	+= (tjs_int)value.AsInteger();
			getValue(obj, L"parent", value);
			obj		= value.AsObjectNoAddRef();
		}
		while(obj != NULL);
		return tmp;
	}

	//	���C���[�̃E�B���h�E�̃E�B���h�E�n���h�������߂�
	static HWND	getLayerWindowHWND(iTJSDispatch2 * obj)
	{
		tTJSVariant	value;
		getValue(obj, L"window", value);
		obj	= value.AsObjectNoAddRef();
		getValue(obj, L"HWND", value);
		return (HWND)value.AsInteger();
	}

	//	���C���[�̊g��{�����擾
	static tjs_real getWindowZoom(iTJSDispatch2 * obj)
	{
		tTJSVariant	value;
		getValue(obj, L"window", value);
		obj	= value.AsObjectNoAddRef();
		getValue(obj, L"zoomNumer", value);
		tjs_int	numer = (tjs_int)value.AsInteger();
		getValue(obj, L"zoomDenom", value);
		tjs_int	denom = (tjs_int)value.AsInteger();
		return (tjs_real)numer / denom;
	}

public:
	LayerExScreen(){}

	//	�X�N���[�����X���W���擾
	static tjs_error TJS_INTF_METHOD screenLeft(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
		if(result == NULL)
			return TJS_S_OK;

		tjs_int	left = getLayerAbsolutePos(objthis, L"left");	//	primaryLayer ��ł̍��W�����߂�
		HWND hWnd = getLayerWindowHWND(objthis);	//	�E�B���h�E�n���h�����擾

		//	���l�̎w�肪����΁A���̕�������
		if(numparams >= 1 && param[0]->Type() == tvtInteger)
			left	+= (tjs_int)param[0]->AsInteger();

		//	���C���[�g��{�����ύX����Ă���΁A���̕����W��ύX����
		left	= (tjs_int)(left * getWindowZoom(objthis));

		//	�X�N���[����̍��W�֕ϊ����ĕԂ�
		POINT	pt;
		pt.x	= left;
		pt.y	= 0;
		ClientToScreen(hWnd, &pt);
		*result	= tTVInteger(pt.x);

		return TJS_S_OK;
	}

	//	�X�N���[�����Y���W���擾
	static tjs_error TJS_INTF_METHOD screenTop(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
		if(result == NULL)
			return TJS_S_OK;

		tjs_int	top = getLayerAbsolutePos(objthis, L"top");	//	primaryLayer ��ł̍��W�����߂�
		HWND hWnd = getLayerWindowHWND(objthis);	//	�E�B���h�E�n���h�����擾

		//	���l�̎w�肪����΁A���̕�������
		if(numparams >= 1 && param[0]->Type() == tvtInteger)
			top	+= (tjs_int)param[0]->AsInteger();

		//	���C���[�g��{�����ύX����Ă���΁A���̕����W��ύX����
		top		= (tjs_int)(top * getWindowZoom(objthis));

		//	�X�N���[����̍��W�֕ϊ����ĕԂ�
		POINT	pt;
		pt.x	= 0;
		pt.y	= top;
		ClientToScreen(hWnd, &pt);
		*result	= tTVInteger(pt.y);

		return TJS_S_OK;
	}
};


NCB_ATTACH_CLASS(LayerExScreen, Layer)
{
	//	�����I�ɂ̓v���p�e�B�ɂ�����
	RawCallback("getScreenLeft", &Class::screenLeft, 0);
	RawCallback("getScreenTop", &Class::screenTop, 0);
};

