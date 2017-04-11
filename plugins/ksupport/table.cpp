#include "ncbind.hpp"

tjs_uint32 posHint, sizeHint, addHint;

//----------------------------------------------------------------------
// �z����쐬
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

//----------------------------------------------------------------------
// �o�C���h
NCB_REGISTER_FUNCTION(table_find_list_range, table_find_list_range);
