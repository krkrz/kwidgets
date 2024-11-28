#define NOMINMAX
#include <windows.h>
#include <vector>
#include <stack>
#include "tp_stub.h"
#include "ncbind.hpp"

static tjs_uint32 countHint, addHint, assignHint, sortHint, reverseHint;


static tTJSVariant createArray(void)
{
	tTJSVariant result;
	TVPExecuteExpression(L"[]", &result);
	return result;
}

static tjs_uint countArray(tTJSVariant array) {
	ncbPropAccessor arrayObj(array);
	return arrayObj.GetValue(L"count", ncbTypedefs::Tag<tjs_uint>(), 0, &countHint);
}

static void assignArray(tTJSVariant &from, tTJSVariant &to) {
	ncbPropAccessor toObj(to);
	toObj.FuncCall(0, L"assign", &assignHint, NULL, from);
}

class ArraySupport
{
private:
	iTJSDispatch2 *mObjthis;

public:
	ArraySupport(iTJSDispatch2 *objthis) : mObjthis(objthis) {}

	void assign(tTJSVariant src) {
		ncbPropAccessor arrayObj(mObjthis, mObjthis);
		arrayObj.FuncCall(0, L"assign", &assignHint, NULL, src);
	}

	tjs_uint count(void) const {
		ncbPropAccessor arrayObj(mObjthis, mObjthis);
		return arrayObj.GetValue(L"count", ncbTypedefs::Tag<tjs_uint>(), 0, &countHint);
	}

	tTJSVariant _uniq(void) {
		ncbPropAccessor arrayObj(mObjthis);
		auto arrayObjCount = count();

		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		tTJSVariant prevElm;

		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			if (i == 0
				|| prevElm.DiscernCompare(elm) == 0)
				resultObj.FuncCall(0, L"add", &addHint, NULL, elm);
			prevElm = elm;
		}
		return result;
	}

	void uniq(void) {
		assign(_uniq());
	}

	static tTJSVariant __map__(tTJSVariant array, tTJSVariant func, std::vector<tTJSVariant*> &args) {
		ncbPropAccessor arrayObj(array);
		auto arrayObjCount = countArray(array);

		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		auto &funcClosure = func.AsObjectClosureNoAddRef();

		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			tTJSVariant funcResult;
			args[0] = &elm;
			funcClosure.FuncCall(0, NULL, NULL, &funcResult, tjs_int(args.size()), &args[0], NULL);
			resultObj.FuncCall(0, L"add", &addHint, NULL, funcResult);
		}

		return result;
	}

	static tjs_error TJS_INTF_METHOD _map(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1)
			return TJS_E_BADPARAMCOUNT;
		tTJSVariant func = *param[numparams - 1];
		std::vector<tTJSVariant*> args(1);
		for (tjs_int i = 0; i < numparams - 1; i++)
			args.push_back(param[i]);
		if (result)
			*result = __map__(tTJSVariant(objthis, objthis), func, args);
		return TJS_S_OK;
	}

	static tjs_error TJS_INTF_METHOD map(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		tTJSVariant resultArray;
		auto resultStatus = _map(&resultArray, numparams, param, objthis);
		if (resultStatus == TJS_E_BADPARAMCOUNT)
			return resultStatus;
		tTJSVariant arrayObj(objthis, objthis);
		assignArray(resultArray, arrayObj);
		return TJS_S_OK;
	}

	static tTJSVariant __filterMap__(tTJSVariant array, tTJSVariant func, std::vector<tTJSVariant*> &args) {
		ncbPropAccessor arrayObj(array);
		auto arrayObjCount = countArray(array);

		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		auto &funcClosure = func.AsObjectClosureNoAddRef();

		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			tTJSVariant funcResult;
			args[0] = &elm;
			funcClosure.FuncCall(0, NULL, NULL, &funcResult, tjs_int(args.size()), &args[0], NULL);
			if (funcResult.Type() != tvtVoid)
				resultObj.FuncCall(0, L"add", &addHint, NULL, funcResult);
		}

		return result;
	}

	static tjs_error TJS_INTF_METHOD _filterMap(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1)
			return TJS_E_BADPARAMCOUNT;
		tTJSVariant func = *param[numparams - 1];
		std::vector<tTJSVariant*> args(1);
		for (tjs_int i = 0; i < numparams - 1; i++)
			args.push_back(param[i]);
		if (result)
			*result = __filterMap__(tTJSVariant(objthis, objthis), func, args);
		return TJS_S_OK;
	}

	static tjs_error TJS_INTF_METHOD filterMap(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		tTJSVariant resultArray;
		auto resultStatus = _filterMap(&resultArray, numparams, param, objthis);
		if (resultStatus == TJS_E_BADPARAMCOUNT)
			return resultStatus;
		tTJSVariant arrayObj(objthis, objthis);
		assignArray(resultArray, arrayObj);
		return TJS_S_OK;
	}

	static tTJSVariant __select__(tTJSVariant array, tTJSVariant func, std::vector<tTJSVariant*> &args) {
		ncbPropAccessor arrayObj(array);
		auto arrayObjCount = countArray(array);

		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		auto &funcClosure = func.AsObjectClosureNoAddRef();

		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			tTJSVariant funcResult;
			args[0] = &elm;
			funcClosure.FuncCall(0, NULL, NULL, &funcResult, tjs_int(args.size()), &args[0], NULL);
			if (tjs_int(funcResult))
				resultObj.FuncCall(0, L"add", &addHint, NULL, elm);
		}

		return result;
	}

	static tjs_error TJS_INTF_METHOD _select(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1)
			return TJS_E_BADPARAMCOUNT;
		tTJSVariant func = *param[numparams - 1];
		std::vector<tTJSVariant*> args(1);
		for (tjs_int i = 0; i < numparams - 1; i++)
			args.push_back(param[i]);
		if (result)
			*result = __select__(tTJSVariant(objthis, objthis), func, args);
		return TJS_S_OK;
	}

	static tjs_error TJS_INTF_METHOD select(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		tTJSVariant resultArray;
		auto resultStatus = _select(&resultArray, numparams, param, objthis);
		if (resultStatus == TJS_E_BADPARAMCOUNT)
			return resultStatus;
		tTJSVariant arrayObj(objthis, objthis);
		assignArray(resultArray, arrayObj);
		return TJS_S_OK;
	}

	static tTJSVariant __reject__(tTJSVariant array, tTJSVariant func, std::vector<tTJSVariant*> &args) {
		ncbPropAccessor arrayObj(array);
		auto arrayObjCount = countArray(array);

		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		auto &funcClosure = func.AsObjectClosureNoAddRef();

		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			tTJSVariant funcResult;
			args[0] = &elm;
			funcClosure.FuncCall(0, NULL, NULL, &funcResult, tjs_int(args.size()), &args[0], NULL);
			if (! tjs_int(funcResult))
				resultObj.FuncCall(0, L"add", &addHint, NULL, elm);
		}

		return result;
	}

	static tjs_error TJS_INTF_METHOD _reject(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1)
			return TJS_E_BADPARAMCOUNT;
		tTJSVariant func = *param[numparams - 1];
		std::vector<tTJSVariant*> args(1);
		for (tjs_int i = 0; i < numparams - 1; i++)
			args.push_back(param[i]);
		if (result)
			*result = __reject__(tTJSVariant(objthis, objthis), func, args);
		return TJS_S_OK;
	}

	static tjs_error TJS_INTF_METHOD reject(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		tTJSVariant resultArray;
		auto resultStatus = _reject(&resultArray, numparams, param, objthis);
		if (resultStatus == TJS_E_BADPARAMCOUNT)
			return resultStatus;
		tTJSVariant arrayObj(objthis, objthis);
		assignArray(resultArray, arrayObj);
		return TJS_S_OK;
	}

	tTJSVariant inject(tTJSVariant init, tTJSVariant func) {
		ncbPropAccessor arrayObj(mObjthis);
		auto arrayObjCount = count();

		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		auto &funcClosure = func.AsObjectClosureNoAddRef();

		tTJSVariant value = init;

		tTJSVariant *params[2];
		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			tTJSVariant funcResult;
			params[1] = &value;
			params[0] = &elm;
			funcClosure.FuncCall(0, NULL, NULL, &funcResult, 2, params, NULL);
			value = funcResult;
		}

		return value;
	}

	static void _each(tTJSVariant array, tTJSVariant func, std::vector<tTJSVariant*> &args) {
		ncbPropAccessor arrayObj(array);
		auto arrayObjCount = countArray(array);

		auto &funcClosure = func.AsObjectClosureNoAddRef();

		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			args[0] = &elm;
			funcClosure.FuncCall(0, NULL, NULL, NULL, tjs_int(args.size()), &args[0], NULL);
		}
	}

	static tjs_error TJS_INTF_METHOD each(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1)
			return TJS_E_BADPARAMCOUNT;
		tTJSVariant func = *param[numparams - 1];
		std::vector<tTJSVariant*> args(1);
		for (tjs_int i = 0; i < numparams - 1; i++)
			args.push_back(param[i]);
		_each(tTJSVariant(objthis, objthis), func, args);
		return TJS_S_OK;
	}

	static void _eachWithIndex(tTJSVariant array, tTJSVariant func, std::vector<tTJSVariant*> &args) {
		ncbPropAccessor arrayObj(array);
		auto arrayObjCount = countArray(array);

		auto &funcClosure = func.AsObjectClosureNoAddRef();
		tTJSVariant index;

		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			index = tjs_int(i);
			args[0] = &elm;
			args[1] = &index;
			funcClosure.FuncCall(0, NULL, NULL, NULL, tjs_int(args.size()), &args[0], NULL);
		}
	}

	static tjs_error TJS_INTF_METHOD eachWithIndex(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1)
			return TJS_E_BADPARAMCOUNT;
		tTJSVariant func = *param[numparams - 1];
		std::vector<tTJSVariant*> args(2);
		for (tjs_int i = 0; i < numparams - 1; i++)
			args.push_back(param[i]);
		_eachWithIndex(tTJSVariant(objthis, objthis), func, args);
		return TJS_S_OK;
	}

	static bool __all__(tTJSVariant array, tTJSVariant val, std::vector<tTJSVariant*> &args) {
		ncbPropAccessor arrayObj(array);
		auto arrayObjCount = countArray(array);

		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		if (val.Type() == tvtObject) {
			auto &closure = val.AsObjectClosureNoAddRef();

			if (closure.IsInstanceOf(0, NULL, NULL, L"Function", NULL) == TJS_S_TRUE) {
				for (tjs_uint i = 0; i < arrayObjCount; i++) {
					tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
					tTJSVariant funcResult;
					args[0] = &elm;
					closure.FuncCall(0, NULL, NULL, &funcResult, tjs_int(args.size()), &args[0], NULL);
					if (! tjs_int(funcResult))
						return false;
				}
				return true;
			}
		}

		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			if (! val.DiscernCompare(elm))
				return false;
		}

		return true;
	}

	static tjs_error TJS_INTF_METHOD all(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1)
			return TJS_E_BADPARAMCOUNT;
		tTJSVariant val = *param[numparams - 1];
		std::vector<tTJSVariant*> args(1);
		for (tjs_int i = 0; i < numparams - 1; i++)
			args.push_back(param[i]);
		if (result)
			*result = __all__(tTJSVariant(objthis, objthis), val, args);
		return TJS_S_OK;
	}

	static bool __any__(tTJSVariant array, tTJSVariant val, std::vector<tTJSVariant*> &args) {
		ncbPropAccessor arrayObj(array);
		auto arrayObjCount = countArray(array);

		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		if (val.Type() == tvtObject) {
			auto &closure = val.AsObjectClosureNoAddRef();

			if (closure.IsInstanceOf(0, NULL, NULL, L"Function", NULL) == TJS_S_TRUE) {
				for (tjs_uint i = 0; i < arrayObjCount; i++) {
					tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
					tTJSVariant funcResult;
					args[0] = &elm;
					closure.FuncCall(0, NULL, NULL, &funcResult, tjs_int(args.size()), &args[0], NULL);
					if (tjs_int(funcResult))
						return true;
				}
				return false;
			}
		}

		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			if (val.DiscernCompare(elm))
				return true;
		}

		return false;
	}

	static tjs_error TJS_INTF_METHOD any(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1)
			return TJS_E_BADPARAMCOUNT;
		tTJSVariant val = *param[numparams - 1];
		std::vector<tTJSVariant*> args(1);
		for (tjs_int i = 0; i < numparams - 1; i++)
			args.push_back(param[i]);
		if (result)
			*result = __any__(tTJSVariant(objthis, objthis), val, args);
		return TJS_S_OK;
	}

	static tTJSVariant __sort(tTJSVariant array, tTJSVariant cond, bool stable) {
		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		resultObj.FuncCall(0, L"assign", &assignHint, NULL, array);
		resultObj.FuncCall(0, L"sort", &sortHint, NULL, cond, stable);

		return result;
	}

	static tjs_error TJS_INTF_METHOD _sort(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 0
			|| numparams > 2)
			return TJS_E_BADPARAMCOUNT;

		tTJSVariant cond = numparams >= 1 ? *param[0] : L"+";
		bool stable = numparams >= 2 ? tjs_int(*param[1]) : false;
		if (result)
			*result = __sort(tTJSVariant(objthis, objthis), cond, stable);

		return TJS_S_OK;
  }

	static tTJSVariant __reverse(tTJSVariant array) {
		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		resultObj.FuncCall(0, L"assign", &assignHint, NULL, array);
		resultObj.FuncCall(0, L"reverse", &reverseHint, NULL);

		return result;
	}

	static tjs_error TJS_INTF_METHOD _reverse(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams !=  0)
			return TJS_E_BADPARAMCOUNT;

		if (result)
			*result = __reverse(tTJSVariant(objthis, objthis));

		return TJS_S_OK;
	}

	tTJSVariant min(void) {
		ncbPropAccessor arrayObj(mObjthis);
		auto arrayObjCount = count();

		tTJSVariant result;

		if (arrayObjCount == 0) {
			return result;
		}

		result = arrayObj.GetValue(0, ncbTypedefs::Tag<tTJSVariant>());
		for (tjs_uint i = 1; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			if (elm.GreaterThan(result))
				result = elm;
		}

		return result;
	}

	tTJSVariant max(void) {
		ncbPropAccessor arrayObj(mObjthis);
		auto arrayObjCount = count();

		tTJSVariant result;

		if (arrayObjCount == 0) {
			return result;
		}

		result = arrayObj.GetValue(0, ncbTypedefs::Tag<tTJSVariant>());
		for (tjs_uint i = 1; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			if (elm.LittlerThan(result))
				result = elm;
		}

		return result;
	}

	static tjs_int __findIf__(tTJSVariant array, tTJSVariant func, std::vector<tTJSVariant*> &args) {
		ncbPropAccessor arrayObj(array);
		auto arrayObjCount = countArray(array);

		auto &funcClosure = func.AsObjectClosureNoAddRef();

		for (tjs_uint i = 0; i < arrayObjCount; i++) {
			tTJSVariant elm = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			tTJSVariant funcResult;
			args[0] = &elm;
			funcClosure.FuncCall(0, NULL, NULL, &funcResult, tjs_int(args.size()), &args[0], NULL);
			if (tjs_int(funcResult))
				return tjs_int(i);
		}

		return tjs_int(-1);
	}

	static tjs_error TJS_INTF_METHOD findIf(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1)
			return TJS_E_BADPARAMCOUNT;

		tTJSVariant func = *param[numparams - 1];
		std::vector<tTJSVariant*> args(1);
		for (tjs_int i = 0; i < numparams - 1; i++)
			args.push_back(param[i]);

		if (result)
			*result = __findIf__(tTJSVariant(objthis, objthis), func, args);

		return TJS_S_OK;
	}

	static tTJSVariant __slice__(tTJSVariant array, tjs_int startIndex, tjs_int size) {
		ncbPropAccessor arrayObj(array);
		auto arrayObjCount = countArray(array);

		tTJSVariant result = createArray();
		ncbPropAccessor resultObj(result);

		auto endIndex = (size == 0) ? arrayObjCount : std::min(tjs_uint(startIndex + size), arrayObjCount);

		for (tjs_uint i = startIndex; i < endIndex; i++) {
			auto value = arrayObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
			resultObj.SetValue(i - startIndex, value);
		}

		return result;
	}

	static tjs_error TJS_INTF_METHOD slice(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams > 2)
			return TJS_E_BADPARAMCOUNT;

		tjs_int startIndex = *param[0];
		tjs_int size = (numparams >= 2) ? *param[1] : 0;

		if (result)
			*result = __slice__(tTJSVariant(objthis, objthis), startIndex, size);

		return TJS_S_OK;
	}

	tTJSVariant flatten() {
		auto result = createArray();
		ncbPropAccessor resultObj(result);

		struct ArrayTravel {
			ArrayTravel(iTJSDispatch2* array,
						 tjs_uint count,
						 tjs_uint index)
				: array(array), count(count), index(index) {
			}

			iTJSDispatch2* array;
			tjs_uint count;
			tjs_uint index;
		};
		std::stack<ArrayTravel> arrayTravelStack;

		arrayTravelStack.push(ArrayTravel(mObjthis, count(), 0));

		iTJSDispatch2 *current;
		tjs_uint count = 0;
		tjs_uint index = 0;

		for(;;) {
			if (index >= count) {
				if (arrayTravelStack.empty())
					break;
				auto &fs = arrayTravelStack.top();
				current = fs.array;
				count = fs.count;
				index = fs.index;
				arrayTravelStack.pop();
				continue;
			}
			tTJSVariant elm;
			current->PropGetByNum(0, index, &elm, current);
			if (elm.Type() == tvtObject) {
				auto elmDispatch = elm.AsObjectNoAddRef();
				if (elmDispatch->IsInstanceOf(0, NULL, NULL, L"Array", elmDispatch) == TJS_S_TRUE) {
					arrayTravelStack.push(ArrayTravel(current, count, index + 1));
					current = elmDispatch;
					count = countArray(elm);
					index = 0;
					continue;
				}
			}
			resultObj.FuncCall(0, L"add", &addHint, NULL, elm);
			index++;
		}
		return result;
	}
};


NCB_GET_INSTANCE_HOOK(ArraySupport)
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

NCB_ATTACH_CLASS_WITH_HOOK(ArraySupport, Array) {
	NCB_METHOD_RAW_CALLBACK(each, ArraySupport::each, 0);
	NCB_METHOD_RAW_CALLBACK(eachWithIndex, ArraySupport::eachWithIndex, 0);
	NCB_METHOD(uniq);
	NCB_METHOD(_uniq);
	NCB_METHOD_RAW_CALLBACK(map, ArraySupport::map, 0);
	NCB_METHOD_RAW_CALLBACK(filterMap, ArraySupport::filterMap, 0);
	NCB_METHOD_RAW_CALLBACK(select, ArraySupport::select, 0);
	NCB_METHOD_RAW_CALLBACK(reject, ArraySupport::reject, 0);
	NCB_METHOD_RAW_CALLBACK(all, ArraySupport::all, 0);
	NCB_METHOD_RAW_CALLBACK(any, ArraySupport::any, 0);
	NCB_METHOD(min);
	NCB_METHOD(max);
	NCB_METHOD_RAW_CALLBACK(_map, ArraySupport::_map, 0);
	NCB_METHOD_RAW_CALLBACK(_filterMap, ArraySupport::_filterMap, 0);
	NCB_METHOD_RAW_CALLBACK(_select, ArraySupport::_select, 0);
	NCB_METHOD_RAW_CALLBACK(_reject, ArraySupport::_reject, 0);
	NCB_METHOD(inject);
	NCB_METHOD_RAW_CALLBACK(_sort, ArraySupport::_sort, 0);
	NCB_METHOD_RAW_CALLBACK(_reverse, ArraySupport::_reverse, 0);
	NCB_METHOD_RAW_CALLBACK(findIf, ArraySupport::findIf, 0);
	NCB_METHOD_RAW_CALLBACK(slice, ArraySupport::slice, 0);
	NCB_METHOD(flatten);
}

