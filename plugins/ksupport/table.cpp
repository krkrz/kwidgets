#include "ncbind.hpp"

static tjs_uint32 posHint, sizeHint, addHint, getTextWidthHint;

//----------------------------------------------------------------------
// 配列を作成
static tTJSVariant createArray(void)
{
  iTJSDispatch2 *obj = TJSCreateArrayObject();
  tTJSVariant result(obj, obj);
  obj->Release();
  return result;
}


tTJSVariant table_find_list_range(tTJSVariant list, tjs_int pos, tjs_int size)
{
  ncbPropAccessor listObj(list);

  tjs_int begin, end, mid, listCount;
  begin = 0;
  end = listCount = listObj.GetArrayCount();

  tTJSVariant result = createArray();

  while (begin < end) {
    mid = int((begin + end) / 2);
    tTJSVariant item = listObj.GetValue(mid, ncbTypedefs::Tag<tTJSVariant>());
    ncbPropAccessor itemObj(item);
    tjs_int itemPos = itemObj.GetValue(L"pos", ncbTypedefs::Tag<tjs_int>(), 8, &posHint);
    tjs_int itemSize = itemObj.GetValue(L"size", ncbTypedefs::Tag<tjs_int>(), 8, &sizeHint);

    if (itemPos <= pos && pos < itemPos + itemSize) {
      ncbPropAccessor resultObj(result);
      resultObj.FuncCall(0, L"add", &addHint, NULL, item);
      for (tjs_int i = mid + 1; i < listCount; i++) {
        tTJSVariant aItem = listObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
        ncbPropAccessor aItemObj(aItem);
        tjs_int aItemPos = aItemObj.GetValue(L"pos", ncbTypedefs::Tag<tjs_int>(), 8, &posHint);
        if (aItemPos >= pos + size)
          break;
        resultObj.FuncCall(0, L"add", &addHint, NULL, aItem);
      }
      return result;
    }
    if (pos < itemPos)
      end = mid;
    else
      begin = mid + 1;
  }

  return result;
}

ttstr substr_ttstr(ttstr src, tjs_int index, tjs_int length)
{
  if (length <= 0
      || index >= src.length())
    return L"";
  if (index + length > src.length())
    length = src.length() - index;
  return ttstr(src.c_str() + index, length);
}

tjs_int getTextWidth(tTJSVariant font, ttstr text)
{
  ncbPropAccessor fontObj(font);
  tTJSVariant result;
  fontObj.FuncCall(0, L"getTextWidth", &getTextWidthHint, &result, text);
  return tjs_int(result);
}

tTJSVariant table_find_text_range(tTJSVariant font, ttstr text, tjs_int w, tjs_int omitTextWidth)
{
  ncbPropAccessor fontObj(font);

  if (getTextWidth(font, text) <= w)
    return tTJSVariant();

  w -= omitTextWidth;
  tjs_int begin, end, mid;
  begin = 0;
  end = text.length();
  mid = (begin + end) / 2;
  tjs_int tw = getTextWidth(font, substr_ttstr(text, 0, mid));
  while (begin < end) {
    if (tw <= w) {
      begin = mid + 1;
      mid = (begin + end) / 2;
      tw += getTextWidth(font, substr_ttstr(text, (begin - 1), mid - (begin - 1)));
    } else {
      end = mid - 1;
      mid = (begin + end) / 2;
      tw -= getTextWidth(font, substr_ttstr(text, mid, (end + 1) - mid));
    }
  }
  return tTJSVariant(mid);
}

//----------------------------------------------------------------------
// バインド
NCB_REGISTER_FUNCTION(table_find_list_range, table_find_list_range);
NCB_REGISTER_FUNCTION(table_find_text_range, table_find_text_range);
