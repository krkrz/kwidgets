//
// $Id$
//



#include "ncbind.hpp"

///----------------------------------------------------------------------
/// 変数
tjs_uint32 timeHint, typeHint ,timeLineFrameWidthHint, timelineFrameHeightHint, fillRectHint, winDarken2Hint, winDarken1Hint, winLighten1Hint, winWhiteHint, canvasHint, fontHeightHint, getTextWidthHint, drawTextHint, frameListHint, drawFrameHint, selectionHint, colorRectHint, layerHint, topHint, oneSecondFrameBgLayerHint, halfSecondFrameBgLayerHint, fifthFrameBgLayerHint, normalFrameBgLayerHint, copyRectHint, operateRectHint, widthHint, heightHint, frameLeftMarkerLayerHint, frameRightMarkerLayerHint, dashLineAppHint, drawLineHint, fontHint, rootHint, ownerHint, framePerSecondHint, fillGradientRectLRHint, setClipHint, frameSignColorHint, fontStyleHint, applyFontStyleHint, drawUITextHint;

tjs_uint32 _singleFrameLeftColorHint, _tweenFrameLeftColorHint, _continuousFrameLeftColorHint;
tjs_uint32 _singleFrameRightColorHint, _tweenFrameRightColorHint, _continuousFrameRightColorHint;

enum {
  TIMELINE_FRAME_TYPE_NULL = 0,
  TIMELINE_FRAME_TYPE_SINGLE = 1,
  TIMELINE_FRAME_TYPE_CONTINUOUS = 2,
  TIMELINE_FRAME_TYPE_TWEEN = 3,
};

enum
{
  TIMELINE_MARKER_MASK_NONE = 0,
  TIMELINE_MARKER_MASK_LEFT = 1,
  TIMELINE_MARKER_MASK_RIGHT = 2,
};


///----------------------------------------------------------------------
/// フレームを検索する
tjs_int timeline_find_frame(tTJSVariant frameList, tjs_int time, bool includeTail)
{
  ncbPropAccessor frameListObj(frameList);

  tjs_int begin, end, mid;

  begin = 0;
  end = frameListObj.GetArrayCount();

  // 末尾だけ特殊処理
  if (end) {
    tTJSVariant lastFrame = frameListObj.GetValue(end - 1, ncbTypedefs::Tag<tTJSVariant>());
    ncbPropAccessor lastFrameObj(lastFrame);
    tjs_int lastTime = lastFrameObj.GetValue(L"time", ncbTypedefs::Tag<tjs_int>(), 0, &timeHint);

    if (! includeTail) {
      if (lastTime == time)
	return end - 1;
      if (lastTime < time)
	return -1;
    } else {
      if (lastTime <= time)
	return end - 1;
    }
  }
  
  // 二分検索
  while (begin < end) {
    mid = tjs_int((begin + end) / 2);

    tTJSVariant curFrame = frameListObj.GetValue(mid, ncbTypedefs::Tag<tTJSVariant>());
    ncbPropAccessor curFrameObj(curFrame);
    tjs_int curTime = curFrameObj.GetValue(L"time", ncbTypedefs::Tag<tjs_int>(), 0, &timeHint);

    if (curTime <= time) {
      tTJSVariant nextFrame = frameListObj.GetValue(mid + 1, ncbTypedefs::Tag<tTJSVariant>());
      ncbPropAccessor nextFrameObj(nextFrame);
      tjs_int nextTime = nextFrameObj.GetValue(L"time", ncbTypedefs::Tag<tjs_int>(), 0, &timeHint);
      if (time < nextTime)
	return mid;
    }
    if (time < curTime)
      end = mid;
    else
      begin = mid + 1;
  }

  return -1;
}


///----------------------------------------------------------------------
/// BGを描画する
void timeline_draw_bg(tTJSVariant item, tTJSVariant view, tjs_int y, tjs_int fromTime, tjs_int toTime)
{
  ncbPropAccessor itemObj(item);
  ncbPropAccessor viewObj(view);

  ncbPropAccessor rootObj(itemObj.GetValue(L"root", ncbTypedefs::Tag<tTJSVariant>(), 0, &rootHint));
  ncbPropAccessor ownerObj(rootObj.GetValue(L"owner", ncbTypedefs::Tag<tTJSVariant>(), 0, &ownerHint));
  tjs_int fps = ownerObj.GetValue(L"framePerSecond", ncbTypedefs::Tag<tjs_int>(), 0 ,&framePerSecondHint);

  iTJSDispatch2 *global = TVPGetScriptDispatch();
  ncbPropAccessor globalObj(global);
  global->Release();

  tjs_int TIMELINE_FRAME_WIDTH = viewObj.GetValue(L"TIMELINE_FRAME_WIDTH", ncbTypedefs::Tag<tjs_int>(), 0, &timeLineFrameWidthHint);
  tjs_int TIMELINE_FRAME_HEIGHT = itemObj.getIntValue(L"height");

  tTJSVariant widgetStyle = viewObj.GetValue(L"widgetStyle", ncbTypedefs::Tag<tTJSVariant>());
  ncbPropAccessor widgetStyleObj(widgetStyle);
  tjs_int oneSecondFrameBgColor = widgetStyleObj.getIntValue(L"oneSecondFrameBgColor");
  tjs_int halfSecondFrameBgColor = widgetStyleObj.getIntValue(L"halfSecondFrameBgColor");
  tjs_int fifthFrameBgColor = widgetStyleObj.getIntValue(L"fifthFrameBgColor");
  tjs_int normalFrameBgColor = widgetStyleObj.getIntValue(L"normalFrameBgColor");
  tjs_int frameBorderColor = widgetStyleObj.getIntValue(L"frameBorderColor");
  
  // バックグラウンドを描画
  for (tjs_int time = fromTime; time < toTime; time ++) {
    tjs_int x = time * TIMELINE_FRAME_WIDTH;
	tjs_int color;
    if (time % fps == 0) {
		color = oneSecondFrameBgColor;
    } else if (time % fps * 2 == fps) {
		color = halfSecondFrameBgColor;
    } else if (time % 5 == 0) {
		color = fifthFrameBgColor;
    } else {
		color = normalFrameBgColor;
    }
	viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL, x, y, TIMELINE_FRAME_WIDTH, TIMELINE_FRAME_HEIGHT, color);
	viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL, x + TIMELINE_FRAME_WIDTH - 1, y, 1, TIMELINE_FRAME_HEIGHT, frameBorderColor);
	viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL, x, y + TIMELINE_FRAME_HEIGHT - 1, TIMELINE_FRAME_WIDTH, 1, frameBorderColor);
  }

  // カーソルを描画
  tjs_int cursorColor = viewObj.getIntValue(L"timelineCursorColor");
  tjs_int cursorX = viewObj.getIntValue(L"timelineCursorX");
  tjs_int cursorY = viewObj.getIntValue(L"timelineCursorY");
  if (fromTime * TIMELINE_FRAME_WIDTH <= cursorX && cursorX < toTime * TIMELINE_FRAME_WIDTH)
	  viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL, cursorX, y, 1, TIMELINE_FRAME_HEIGHT, cursorColor);
  if (y <= cursorY && cursorY < y + TIMELINE_FRAME_HEIGHT)
	  viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL, fromTime * TIMELINE_FRAME_WIDTH, cursorY, (toTime - fromTime) * TIMELINE_FRAME_WIDTH, 1, cursorColor);
}

///----------------------------------------------------------------------
/// フレームを描画する
void timeline_draw_frame(tTJSVariant item, tTJSVariant view, tjs_int layerIndex, tjs_int frameIndex, tjs_int y, tTJSVariant frame, tjs_int length, tjs_uint markerMask)
{
	ncbPropAccessor itemObj(item);
	ncbPropAccessor viewObj(view);
	ncbPropAccessor frameObj(frame);

	iTJSDispatch2 *global = TVPGetScriptDispatch();
	ncbPropAccessor globalObj(global);
	global->Release();

	tjs_int WIN_DARKEN2 = globalObj.GetValue(L"WIN_DARKEN2", ncbTypedefs::Tag<tjs_uint>(), 0, &winDarken2Hint);
	tjs_int TIMELINE_FRAME_WIDTH = viewObj.GetValue(L"TIMELINE_FRAME_WIDTH", ncbTypedefs::Tag<tjs_int>(), 0, &timeLineFrameWidthHint);
	tjs_int TIMELINE_FRAME_HEIGHT = itemObj.GetValue(L"height", ncbTypedefs::Tag<tjs_int>(), 0, &timelineFrameHeightHint);
	tjs_int frameType = frameObj.GetValue(L"type", ncbTypedefs::Tag<tjs_int>(), 0, &typeHint);
	tjs_int frameTime = frameObj.GetValue(L"time", ncbTypedefs::Tag<tjs_int>(), 0, &timeHint);

	switch (frameType) {
	case TIMELINE_FRAME_TYPE_NULL:
		timeline_draw_bg(item, view, y, frameTime, frameTime + length);
		break;
	case TIMELINE_FRAME_TYPE_SINGLE: {

		timeline_draw_bg(item, view, y, frameTime + 1, frameTime + length);
		tjs_int singleFrameLeftColor  = itemObj.GetValue(L"singleFrameLeftColor", ncbTypedefs::Tag<tjs_int>(), 0, &_singleFrameLeftColorHint);
		tjs_int singleFrameRightColor  = itemObj.GetValue(L"singleFrameRightColor", ncbTypedefs::Tag<tjs_int>(), 0, &_singleFrameRightColorHint);
		singleFrameLeftColor |= 0xFF000000;
		singleFrameRightColor |= 0xFF000000;
		viewObj.FuncCall(0, L"fillGradientRectLR", &fillGradientRectLRHint, NULL, 
						 frameTime * TIMELINE_FRAME_WIDTH, y, TIMELINE_FRAME_WIDTH - 1, TIMELINE_FRAME_HEIGHT - 1, singleFrameLeftColor, singleFrameRightColor);
		break;
	}
	case TIMELINE_FRAME_TYPE_CONTINUOUS:
	case TIMELINE_FRAME_TYPE_TWEEN: {
		tjs_int leftColor, rightColor;
		if (frameType == TIMELINE_FRAME_TYPE_CONTINUOUS) {
			leftColor = itemObj.GetValue(L"continuousFrameLeftColor", ncbTypedefs::Tag<tjs_int>(), 0, &_continuousFrameLeftColorHint);
			rightColor = itemObj.GetValue(L"continuousFrameRightColor", ncbTypedefs::Tag<tjs_int>(), 0, &_continuousFrameRightColorHint);
		} else {
			leftColor = itemObj.GetValue(L"tweenFrameLeftColor", ncbTypedefs::Tag<tjs_int>(), 0, &_tweenFrameLeftColorHint);
			rightColor = itemObj.GetValue(L"tweenFrameRightColor", ncbTypedefs::Tag<tjs_int>(), 0, &_tweenFrameRightColorHint);
		}
		leftColor |= 0xFF000000;
		rightColor |= 0xFF000000;
		// BG描画
		viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL,
						 (frameTime + length) * TIMELINE_FRAME_WIDTH - 1, y, 1, TIMELINE_FRAME_HEIGHT, WIN_DARKEN2);
		viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL,
						 frameTime * TIMELINE_FRAME_WIDTH, y + TIMELINE_FRAME_HEIGHT - 1, length * TIMELINE_FRAME_WIDTH, 1, WIN_DARKEN2);
		viewObj.FuncCall(0, L"fillGradientRectLR", &fillGradientRectLRHint, NULL,
						 frameTime * TIMELINE_FRAME_WIDTH, y,
						 length * TIMELINE_FRAME_WIDTH - 1,
						 TIMELINE_FRAME_HEIGHT - 1,
						 leftColor, rightColor);
		bool leftMarker = (markerMask & TIMELINE_MARKER_MASK_LEFT) != 0;
		bool rightMarker = (markerMask & TIMELINE_MARKER_MASK_RIGHT) != 0;
		if (rightMarker && leftMarker && length == 1)
			rightMarker = false;
		// トゥイーンの破線描画
		tjs_int fromTime = frameTime + (leftMarker ? 1 : 0), toTime = frameTime + length - (rightMarker ? 1 : 0);
		if (frameType == TIMELINE_FRAME_TYPE_TWEEN) {
			tTJSVariant dashLineApp = viewObj.GetValue(L"dashLineApp", ncbTypedefs::Tag<tTJSVariant>(), 0, &dashLineAppHint);
			tjs_int frameSignColor = viewObj.GetValue(L"frameSignColor", ncbTypedefs::Tag<tjs_int>(), 0, &frameSignColorHint);
			if (dashLineApp.Type() != tvtVoid) {
				viewObj.FuncCall(0, L"drawLine", &drawLineHint, NULL,
								 dashLineApp, fromTime * TIMELINE_FRAME_WIDTH, y + TIMELINE_FRAME_HEIGHT / 2 - 1, toTime * TIMELINE_FRAME_WIDTH, y + TIMELINE_FRAME_HEIGHT / 2 - 1);
			} else {
				for (tjs_int i = fromTime; i < toTime; i++) {
					viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL,
									 i * TIMELINE_FRAME_WIDTH, y + TIMELINE_FRAME_HEIGHT / 2 - 1, TIMELINE_FRAME_WIDTH / 2 - 2, 1, frameSignColor);
					viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL,
									 i * TIMELINE_FRAME_WIDTH + TIMELINE_FRAME_WIDTH / 2, y + TIMELINE_FRAME_HEIGHT / 2 - 1, TIMELINE_FRAME_WIDTH / 2 - 2, 1, frameSignColor);
				}
			}
		}
		// カーソル表示
		auto selectedLayer = viewObj.getIntValue(L"selectedLayer", -1);
		if (selectedLayer == layerIndex) {
			auto cursorColor = viewObj.getIntValue(L"timelineCursorColor");
			auto cursorY = viewObj.getIntValue(L"timelineCursorY");
			viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL,
							 frameTime * TIMELINE_FRAME_WIDTH, cursorY, length * TIMELINE_FRAME_WIDTH, 1, cursorColor);
		}
		auto selectedTime = viewObj.getIntValue(L"selectedTime");
		if (frameTime <= selectedTime && selectedTime < frameTime + length) {
			auto cursorColor = viewObj.getIntValue(L"timelineCursorColor");
			auto cursorX = viewObj.getIntValue(L"timelineCursorX");
			viewObj.FuncCall(0, L"fillRect", &fillRectHint, NULL,
							 cursorX, y, 1, TIMELINE_FRAME_HEIGHT, cursorColor);
		}
		// 左マーカー描画
		if (leftMarker) {
			tTJSVariant frameLeftMarkerLayer = viewObj.GetValue(L"frameLeftMarkerLayer", ncbTypedefs::Tag<tTJSVariant>(), 0, &frameLeftMarkerLayerHint);
			ncbPropAccessor frameLeftMarkerLayerObj(frameLeftMarkerLayer);
			tjs_int mw = frameLeftMarkerLayerObj.GetValue(L"width", ncbTypedefs::Tag<tjs_int>(), 0, &widthHint);
			tjs_int mh = frameLeftMarkerLayerObj.GetValue(L"height", ncbTypedefs::Tag<tjs_int>(), 0, &heightHint);
			viewObj.FuncCall(0, L"operateRect", &operateRectHint, NULL,
							 frameTime * TIMELINE_FRAME_WIDTH, y, 
							 frameLeftMarkerLayer, 0, 0, mw, mh);
		}
		// 右マーカー描画
		if (rightMarker) {
			tTJSVariant frameRightMarkerLayer = viewObj.GetValue(L"frameRightMarkerLayer", ncbTypedefs::Tag<tTJSVariant>(), 0, &frameRightMarkerLayerHint);
			ncbPropAccessor frameRightMarkerLayerObj(frameRightMarkerLayer);
			tjs_int mw = frameRightMarkerLayerObj.GetValue(L"width", ncbTypedefs::Tag<tjs_int>(), 0, &widthHint);
			tjs_int mh = frameRightMarkerLayerObj.GetValue(L"height", ncbTypedefs::Tag<tjs_int>(), 0, &heightHint);
			viewObj.FuncCall(0, L"operateRect", &operateRectHint, NULL,
							 (frameTime + length) * TIMELINE_FRAME_WIDTH - mw, y, 
							 frameRightMarkerLayer, 0, 0, mw, mh);
		}
		// フレーム数描画
		if (length > 2) {
			tjs_int frameSignColor = viewObj.GetValue(L"frameSignColor", ncbTypedefs::Tag<tjs_int>(), 0, &frameSignColorHint);
			tTJSVariant fontStyle = viewObj.GetValue(L"fontStyle", ncbTypedefs::Tag<tTJSVariant>(), 0, &fontStyleHint);
			tTJSVariant font = viewObj.GetValue(L"font", ncbTypedefs::Tag<tTJSVariant>(), 0, &fontHint);
			ncbPropAccessor fontStyleObj(fontStyle);
			tjs_int fontHeight = fontStyleObj.GetValue(L"fontHeight", ncbTypedefs::Tag<tjs_int>(), 0, &fontHeightHint);
			viewObj.FuncCall(0, L"applyFontStyle", &applyFontStyleHint, NULL, font, fontStyle);
			ttstr text(length);
			tTJSVariant textWidth;
			ncbPropAccessor fontObj(font);
			fontObj.FuncCall(0, L"getTextWidth", &getTextWidthHint, &textWidth,
							 text);
			tjs_int l = frameTime * TIMELINE_FRAME_WIDTH + ((length * TIMELINE_FRAME_WIDTH) - tjs_int(textWidth)) / 2;
			tjs_int t = y + int((TIMELINE_FRAME_HEIGHT - fontHeight) / 2) - 1;
			viewObj.FuncCall(0, L"setClip", &setClipHint, NULL, 
							 l - 1, t - 1, tjs_int(textWidth) + 2, fontHeight + 2);
			viewObj.FuncCall(0, L"fillGradientRectLR", &fillGradientRectLRHint, NULL,
							 frameTime * TIMELINE_FRAME_WIDTH, y,
							 length * TIMELINE_FRAME_WIDTH - 1,
							 TIMELINE_FRAME_HEIGHT - 1,
							 leftColor, rightColor);
			viewObj.FuncCall(0, L"setClip", &setClipHint, NULL); 
			viewObj.FuncCall(0, L"drawUIText", &drawUITextHint, NULL,
							 fontStyle, l, t, text, tjs_int(frameSignColor & 0xFFFFFF));
		}
	}
	}

	tTJSVariant matchResult;
	viewObj.FuncCall(0, L"matchSelection", NULL, &matchResult, layerIndex, frameIndex);
	if (tjs_int(matchResult)) {
		tjs_int selectedFrameBlend = viewObj.getIntValue(L"selectedFrameBlend");
		auto selectedFrameColor = viewObj.GetValue(L"selectedFrameColor", ncbTypedefs::Tag<tTJSVariant>());
		viewObj.FuncCall(0, L"operateColorRect", NULL, NULL, frameTime * TIMELINE_FRAME_WIDTH, y, length * TIMELINE_FRAME_WIDTH - 1, TIMELINE_FRAME_HEIGHT - 1, selectedFrameBlend, selectedFrameColor);
	}

	viewObj.FuncCall(0, L"matchPoint", NULL, &matchResult, layerIndex, frameIndex);
	if (tjs_int(matchResult)) {
		tjs_int hoverFrameBlend = viewObj.getIntValue(L"hoverFrameBlend");
		auto hoverFrameColor = viewObj.GetValue(L"hoverFrameColor", ncbTypedefs::Tag<tTJSVariant>());
		viewObj.FuncCall(0, L"operateColorRect", NULL, NULL, frameTime * TIMELINE_FRAME_WIDTH, y, length * TIMELINE_FRAME_WIDTH - 1, TIMELINE_FRAME_HEIGHT - 1, hoverFrameBlend, hoverFrameColor);
	}
}

///----------------------------------------------------------------------
// タイムラインを描画する
  void timeline_draw_timeline(tTJSVariant item, tTJSVariant view, tjs_int layerIndex, tjs_int fromTime, tjs_int toTime) 
{
  ncbPropAccessor itemObj(item);
  ncbPropAccessor viewObj(view);
  tTJSVariant layer = itemObj.GetValue(L"layer", ncbTypedefs::Tag<tTJSVariant>(), 0, &layerHint);
  ncbPropAccessor layerObj(layer);

  iTJSDispatch2 *global = TVPGetScriptDispatch();
  ncbPropAccessor globalObj(global);
  global->Release();

  tjs_int y = layerObj.GetValue(L"top", ncbTypedefs::Tag<tjs_int>(), 0, &topHint);
  tTJSVariant frameList = itemObj.GetValue(L"frameList", ncbTypedefs::Tag<tTJSVariant>(), 0, &frameListHint);
  ncbPropAccessor frameListObj(frameList);
  tjs_int frameListCount = frameListObj.GetArrayCount();

  // 描画範囲を確定
  tjs_int fromFrame = timeline_find_frame(frameList, fromTime, true);
  tjs_int toFrame = timeline_find_frame(frameList, toTime, true);

  // バックグラウンドを描画
  if (fromFrame < 0 && toFrame < 0) {
    timeline_draw_bg(item, view, y, fromTime, toTime);
  } else {
    if (fromFrame < 0) 
      timeline_draw_bg(item, view, y, fromTime, ncbPropAccessor(frameListObj.GetValue(0, ncbTypedefs::Tag<tTJSVariant>())).GetValue(L"time", ncbTypedefs::Tag<tjs_int>(), 0, &timeHint));
    if (toFrame == frameListCount - 1) 
      timeline_draw_bg(item, view, y, ncbPropAccessor(frameListObj.GetValue(-1, ncbTypedefs::Tag<tTJSVariant>())).GetValue(L"time", ncbTypedefs::Tag<tjs_int>(), 0, &timeHint), toTime);
  }

  // フレームを描画
  for (tjs_int i = fromFrame; i <= toFrame; i++) {
    if (i == -1)
      continue;
    tTJSVariant frame = frameListObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
    ncbPropAccessor frameObj(frame);
    tjs_int headTime, tailTime;
    headTime = frameObj.GetValue(L"time", ncbTypedefs::Tag<tjs_int>(), 0, &timeHint);
    if (i < frameListCount - 1) {
      tTJSVariant nextFrame = frameListObj.GetValue(i + 1, ncbTypedefs::Tag<tTJSVariant>());
      ncbPropAccessor nextFrameObj(nextFrame);
      tailTime = nextFrameObj.GetValue(L"time", ncbTypedefs::Tag<tjs_int>(), 0, &timeHint);
    } else 
      tailTime = headTime;
    if (! (tailTime < fromTime
	   || headTime > toTime)) {
      itemObj.FuncCall(0, L"drawFrame", &drawFrameHint, NULL,
					   view, layerIndex, i, y, frame, tailTime - headTime);
    }
  }
}

//----------------------------------------------------------------------
// バインド
NCB_REGISTER_FUNCTION(timeline_find_frame, timeline_find_frame);
NCB_REGISTER_FUNCTION(timeline_draw_bg, timeline_draw_bg);
NCB_REGISTER_FUNCTION(timeline_draw_frame, timeline_draw_frame);
NCB_REGISTER_FUNCTION(timeline_draw_timeline, timeline_draw_timeline);
