//
// $Id$
//


#include <windows.h>
#undef max
#undef min

#include <vector>
#include <algorithm>
#include <iterator>
#include "ncbind.hpp"


//----------------------------------------------------------------------
// 各種ユーティリティ関数群
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// 変数
tjs_uint32 countHint;

//----------------------------------------------------------------------
// 宣言
bool equalStruct(tTJSVariant v1, tTJSVariant v2);
bool equalStructNumericLoose(tTJSVariant v1, tTJSVariant v2);

//----------------------------------------------------------------------
// 辞書を作成
tTJSVariant createDictionary(void)
{
	iTJSDispatch2 *obj = TJSCreateDictionaryObject();
	tTJSVariant result(obj, obj);
	obj->Release();
	return result;
}

//----------------------------------------------------------------------
// 配列を作成
tTJSVariant createArray(void)
{
	iTJSDispatch2 *obj = TJSCreateArrayObject();
	tTJSVariant result(obj, obj);
	obj->Release();
	return result;
}

//----------------------------------------------------------------------
// 配列をカウント
static tjs_uint
countArray(tTJSVariant array)
{
	static tjs_uint32 countHint;
	ncbPropAccessor arrayObj(array);
	return arrayObj.GetValue(L"count", ncbTypedefs::Tag<tjs_uint>(), 0, &countHint);
}

//----------------------------------------------------------------------
// 辞書の要素を全比較するCaller
class DictMemberCompareCaller : public tTJSDispatch
{
public:
	tTJSVariantClosure &target;
	bool match;

	DictMemberCompareCaller(tTJSVariantClosure &_target)
		: target(_target)
		, match(true) {
	}

	virtual tjs_error TJS_INTF_METHOD FuncCall
	( // function invocation
	 tjs_uint32 flag,			// calling flag
	 const tjs_char * membername,// member name ( NULL for a default member )
	 tjs_uint32 *hint,			// hint for the member name (in/out)
	 tTJSVariant *result,		// result
	 tjs_int numparams,			// number of parameters
	 tTJSVariant **param,		// parameters
	 iTJSDispatch2 *objthis		// object as "this"
	  ) {
		if (result)
			*result = true;
		if (numparams > 1) {
			if ((int)*param[1] != TJS_HIDDENMEMBER) {
				const tjs_char *key = param[0]->GetString();
				tTJSVariant value = *param[2];
				tTJSVariant targetValue;
				if (target.PropGet(0, key, NULL, &targetValue, NULL)
					== TJS_S_OK) {
					match = match && equalStruct(value, targetValue);
					if (result)
						*result = match;
				}
			}
		}
		return TJS_S_OK;
	}
};

//----------------------------------------------------------------------
// 構造体比較関数
bool equalStruct(tTJSVariant v1, tTJSVariant v2)
{
	// タイプがオブジェクトなら特殊判定
	if (v1.Type() == tvtObject
		&& v2.Type() == tvtObject) {
		if (v1.AsObjectNoAddRef() == v2.AsObjectNoAddRef())
			return true;

		tTJSVariantClosure &o1 = v1.AsObjectClosureNoAddRef();
		tTJSVariantClosure &o2 = v2.AsObjectClosureNoAddRef();

		// 関数どうしなら特別扱いで関数比較
		if (o1.IsInstanceOf(0, NULL, NULL, L"Function", NULL)== TJS_S_TRUE
			&& o2.IsInstanceOf(0, NULL, NULL, L"Function", NULL)== TJS_S_TRUE)
			return v1.DiscernCompare(v2);

		// Arrayどうしなら全項目を比較
		if (o1.IsInstanceOf(0, NULL, NULL, L"Array", NULL)== TJS_S_TRUE
			&& o2.IsInstanceOf(0, NULL, NULL, L"Array", NULL)== TJS_S_TRUE) {
			// 長さが一致していなければ比較失敗
			tTJSVariant o1Count, o2Count;
			(void)o1.PropGet(0, L"count", &countHint, &o1Count, NULL);
			(void)o2.PropGet(0, L"count", &countHint, &o2Count, NULL);
			if (! o1Count.DiscernCompare(o2Count))
				return false;
			// 全項目を順番に比較
			tjs_int count = o1Count;
			tTJSVariant o1Val, o2Val;
			for (tjs_int i = 0; i < count; i++) {
				(void)o1.PropGetByNum(TJS_IGNOREPROP, i, &o1Val, NULL);
				(void)o2.PropGetByNum(TJS_IGNOREPROP, i, &o2Val, NULL);
				if (! equalStruct(o1Val, o2Val))
					return false;
			}
			return true;
		}

		// Dictionaryどうしなら全項目を比較
		if (o1.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL)== TJS_S_TRUE
			&& o2.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL)== TJS_S_TRUE) {
			// 項目数が一致していなければ比較失敗
			tjs_int o1Count, o2Count;
			(void)o1.GetCount(&o1Count, NULL, NULL, NULL);
			(void)o2.GetCount(&o2Count, NULL, NULL, NULL);
			if (o1Count != o2Count)
				return false;
			// 全項目を順番に比較
			DictMemberCompareCaller *caller = new DictMemberCompareCaller(o2);
			tTJSVariantClosure closure(caller);
			tTJSVariant(o1.EnumMembers(TJS_IGNOREPROP, &closure, NULL));
			bool result = caller->match;
			caller->Release();
			return result;
		}
	}

	return v1.DiscernCompare(v2);
}

//----------------------------------------------------------------------
// 辞書の要素を全比較するCaller(数字の比較はゆるい)
class DictMemberCompareNumericLooseCaller : public tTJSDispatch
{
public:
	tTJSVariantClosure &target;
	bool match;

	DictMemberCompareNumericLooseCaller(tTJSVariantClosure &_target)
		: target(_target)
		, match(true) {
	}

	virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
											   tjs_uint32 flag,			// calling flag
											   const tjs_char * membername,// member name ( NULL for a default member )
											   tjs_uint32 *hint,			// hint for the member name (in/out)
											   tTJSVariant *result,		// result
											   tjs_int numparams,			// number of parameters
											   tTJSVariant **param,		// parameters
											   iTJSDispatch2 *objthis		// object as "this"
												) {
		if (result)
			*result = true;
		if (numparams > 1) {
			if ((int)*param[1] != TJS_HIDDENMEMBER) {
				const tjs_char *key = param[0]->GetString();
				tTJSVariant value = *param[2];
				tTJSVariant targetValue;
				if (target.PropGet(0, key, NULL, &targetValue, NULL)
					== TJS_S_OK) {
					match = match && equalStructNumericLoose(value, targetValue);
					if (result)
						*result = match;
				}
			}
		}
		return TJS_S_OK;
	}
};

//----------------------------------------------------------------------
// 構造体比較関数(数字の比較はゆるい)
bool equalStructNumericLoose(tTJSVariant v1, tTJSVariant v2)
{
	// タイプがオブジェクトなら特殊判定
	if (v1.Type() == tvtObject
		&& v2.Type() == tvtObject) {
		if (v1.AsObjectNoAddRef() == v2.AsObjectNoAddRef()) {
			return true;
		}

		tTJSVariantClosure &o1 = v1.AsObjectClosureNoAddRef();
		tTJSVariantClosure &o2 = v2.AsObjectClosureNoAddRef();

		// 関数どうしなら特別扱いで関数比較
		if (o1.IsInstanceOf(0, NULL, NULL, L"Function", NULL)== TJS_S_TRUE
			&& o2.IsInstanceOf(0, NULL, NULL, L"Function", NULL)== TJS_S_TRUE) {

			return v1.DiscernCompare(v2);
		}

		// Arrayどうしなら全項目を比較
		if (o1.IsInstanceOf(0, NULL, NULL, L"Array", NULL)== TJS_S_TRUE
			&& o2.IsInstanceOf(0, NULL, NULL, L"Array", NULL)== TJS_S_TRUE) {
			// 長さが一致していなければ比較失敗
			tTJSVariant o1Count, o2Count;
			(void)o1.PropGet(0, L"count", &countHint, &o1Count, NULL);
			(void)o2.PropGet(0, L"count", &countHint, &o2Count, NULL);
			if (! o1Count.DiscernCompare(o2Count)) {
				return false;
			}
			// 全項目を順番に比較
			tjs_int count = o1Count;
			tTJSVariant o1Val, o2Val;
			for (tjs_int i = 0; i < count; i++) {
				(void)o1.PropGetByNum(TJS_IGNOREPROP, i, &o1Val, NULL);
				(void)o2.PropGetByNum(TJS_IGNOREPROP, i, &o2Val, NULL);
				if (! equalStructNumericLoose(o1Val, o2Val)) {
					return false;
				}
			}
			return true;
		}

		// Dictionaryどうしなら全項目を比較
		if (o1.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL)== TJS_S_TRUE
			&& o2.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL)== TJS_S_TRUE) {
			// 項目数が一致していなければ比較失敗
			tjs_int o1Count, o2Count;
			(void)o1.GetCount(&o1Count, NULL, NULL, NULL);
			(void)o2.GetCount(&o2Count, NULL, NULL, NULL);
			if (o1Count != o2Count) {
				return false;
			}
			// 全項目を順番に比較
			DictMemberCompareNumericLooseCaller *caller = new DictMemberCompareNumericLooseCaller(o2);
			tTJSVariantClosure closure(caller);
			tTJSVariant(o1.EnumMembers(TJS_IGNOREPROP, &closure, NULL));
			bool result = caller->match;
			caller->Release();
			return result;
		}
	}

	// Real同士ならfloatに変換して比較
	if (v1.Type() == tvtReal && v2.Type() == tvtReal) {
		return  (fabs(float(tjs_real(v1)) - float(tjs_real(v2))) < FLT_EPSILON);
	}

	// 数字の場合は
	if ((v1.Type() == tvtInteger || v1.Type() == tvtReal)
		&& (v2.Type() == tvtInteger || v2.Type() == tvtReal)) {
		return v1.NormalCompare(v2);
	}

	return v1.DiscernCompare(v2);
}

//----------------------------------------------------------------------
// 辞書のキーを配列で返すCaller
class DictKeysCountCaller : public tTJSDispatch
{
public:
	tTJSVariantClosure &dst;
	tjs_int index;

	DictKeysCountCaller(tTJSVariantClosure &_dst)
		: dst(_dst)
		, index(0) {
	}

	virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
											   tjs_uint32 flag,			// calling flag
											   const tjs_char * membername,// member name ( NULL for a default member )
											   tjs_uint32 *hint,			// hint for the member name (in/out)
											   tTJSVariant *result,		// result
											   tjs_int numparams,			// number of parameters
											   tTJSVariant **param,		// parameters
											   iTJSDispatch2 *objthis		// object as "this"
												) {
		if (numparams > 1) {
			if ((int)*param[1] != TJS_HIDDENMEMBER) {
				const tjs_char *key = param[0]->GetString();
				dst.PropSetByNum(0, index++, param[0], NULL);
			}
		}
		if (result)
			*result = true;
		return TJS_S_OK;
	}
};

//----------------------------------------------------------------------
// 辞書のキーを配列にして返す
tTJSVariant dictionaryKeys(tTJSVariant dictionary)
{
	tTJSVariant result = createArray();
	tTJSVariantClosure &src = dictionary.AsObjectClosureNoAddRef();
	tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
	DictKeysCountCaller *caller = new DictKeysCountCaller(dst);
	tTJSVariantClosure closure(caller);
	src.EnumMembers(TJS_IGNOREPROP, &closure, NULL);
	caller->Release();
	return result;
}

//----------------------------------------------------------------------
// 辞書の値を配列で返すCaller
class DictValuesCountCaller : public tTJSDispatch
{
public:
	tTJSVariantClosure &dst;
	tjs_int index;

	DictValuesCountCaller(tTJSVariantClosure &_dst)
		: dst(_dst)
		, index(0) {
	}

	virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
											   tjs_uint32 flag,			// calling flag
											   const tjs_char * membername,// member name ( NULL for a default member )
											   tjs_uint32 *hint,			// hint for the member name (in/out)
											   tTJSVariant *result,		// result
											   tjs_int numparams,			// number of parameters
											   tTJSVariant **param,		// parameters
											   iTJSDispatch2 *objthis		// object as "this"
												) {
		if (numparams > 1) {
			if ((int)*param[1] != TJS_HIDDENMEMBER) {
				const tjs_char *key = param[0]->GetString();
				dst.PropSetByNum(0, index++, param[2], NULL);
			}
		}
		if (result)
			*result = true;
		return TJS_S_OK;
	}
};  

//----------------------------------------------------------------------
// 辞書の値を配列にして返す
tTJSVariant dictionaryValues(tTJSVariant dictionary)
{
	tTJSVariant result = createArray();
	tTJSVariantClosure &src = dictionary.AsObjectClosureNoAddRef();
	tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
	DictValuesCountCaller *caller = new DictValuesCountCaller(dst);
	tTJSVariantClosure closure(caller);
	src.EnumMembers(TJS_IGNOREPROP, &closure, NULL);
	caller->Release();
	return result;
}

//----------------------------------------------------------------------
// 配列の値をキーに持つ辞書を返す。
tTJSVariant arrayHash(tTJSVariant array)
{
	tTJSVariant result = createDictionary();
	tTJSVariantClosure &src = array.AsObjectClosureNoAddRef();
	tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();

	tTJSVariant arrayCount;
	(void)src.PropGet(0, L"count", &countHint, &arrayCount, NULL);
	tjs_int count = arrayCount;
	tTJSVariant key;
	tTJSVariant value(true);
	for (tjs_int i = 0; i < count; i++) {
		(void)src.PropGetByNum(TJS_IGNOREPROP, i, &key, NULL);
		ttstr keyStr = key;
		(void)dst.PropSet(TJS_MEMBERENSURE, keyStr.c_str(), NULL, &value, NULL);
	}
	return result;
}

//----------------------------------------------------------------------
tTJSVariant _unionDictionary(tTJSVariant v1, tTJSVariant v2, bool recursive);

//----------------------------------------------------------------------
// 辞書を結合するcaller

class DictUnionCaller : public tTJSDispatch
{
public:
	tTJSVariantClosure &dst;
	bool recursive;

	DictUnionCaller(tTJSVariantClosure &_dst, bool _recursive)
		: dst(_dst)
		, recursive(_recursive)
	{
	}

	virtual tjs_error TJS_INTF_METHOD FuncCall
	( // function invocation
	 tjs_uint32 flag,			// calling flag
	 const tjs_char * membername,// member name ( NULL for a default member )
	 tjs_uint32 *hint,			// hint for the member name (in/out)
	 tTJSVariant *result,		// result
	 tjs_int numparams,			// number of parameters
	 tTJSVariant **param,		// parameters
	 iTJSDispatch2 *objthis		// object as "this"
	  ) {
		if (numparams > 1) {
			if ((int)*param[1] != TJS_HIDDENMEMBER) {
				const tjs_char *key = param[0]->GetString();
				tTJSVariant value = *param[2];
				if (recursive
					&& value.Type() == tvtObject
					&& value.AsObjectClosureNoAddRef().IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL) == TJS_S_TRUE) {
					tTJSVariant dstValue;
					if (dst.PropGet(0, key, NULL, &dstValue, NULL) == TJS_S_OK
						&& dstValue.Type() == tvtObject
						&& dstValue.AsObjectClosureNoAddRef().IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL) == TJS_S_TRUE) {
						value = _unionDictionary(dstValue, value, recursive);
					}
				}
				(void)dst.PropSet(TJS_MEMBERENSURE, key, NULL, &value, NULL);
			}
		}
		if (result)
			*result = true;
		return TJS_S_OK;
	}
};

//----------------------------------------------------------------------
// 辞書を結合した辞書を返す
tTJSVariant _unionDictionary(tTJSVariant v1, tTJSVariant v2, bool recursive)
{
	tTJSVariant result = createDictionary();
	tTJSVariantClosure &o1 = v1.AsObjectClosureNoAddRef();
	tTJSVariantClosure &o2 = v2.AsObjectClosureNoAddRef();
	tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();

	auto caller1 = new DictUnionCaller(dst, false);
	tTJSVariantClosure closure1(caller1);
	o1.EnumMembers(TJS_IGNOREPROP, &closure1, NULL);
	caller1->Release();

	auto caller2 = new DictUnionCaller(dst, recursive);
	tTJSVariantClosure closure2(caller2);
	o2.EnumMembers(TJS_IGNOREPROP, &closure2, NULL);
	caller2->Release();

	return result;
}

tjs_error TJS_INTF_METHOD unionDictionary(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis)
{
	if (numparams < 2
		|| numparams > 3)
		return TJS_E_BADPARAMCOUNT;
	tTJSVariant v1 = *(param[0]);
	tTJSVariant v2 = *(param[1]);
	bool recursive = false;
	if (numparams == 3)
		recursive = param[2]->AsInteger() != 0;

	if (! result)
		return TJS_S_OK;

	*result = _unionDictionary(v1, v2, recursive);
	return TJS_S_OK;
}

//----------------------------------------------------------------------
// 配列の中身をvectorに展開
void array_to_vector(tTJSVariant &array, std::vector<tTJSVariant> &vec)
{
	tTJSVariantClosure &obj = array.AsObjectClosureNoAddRef();
	tTJSVariant objCount;
	(void)obj.PropGet(0, L"count", &countHint, &objCount, NULL);
	tjs_int count = objCount;
	vec.reserve(count);
	tTJSVariant value;
	for (tjs_int i = 0; i < count; i++) {
		(void)obj.PropGetByNum(TJS_IGNOREPROP, i, &value, NULL);
		vec.push_back(value);
	}
}

//----------------------------------------------------------------------
// TJSVariant比較
bool TJSVariantCompare(const tTJSVariant &v1, const tTJSVariant &v2)
{
	return v1.LittlerThan(v2);
}

//----------------------------------------------------------------------
// 和集合
tTJSVariant unionSet(tTJSVariant v1, tTJSVariant v2)
{
	std::vector<tTJSVariant> a1, a2, r;

	array_to_vector(v1, a1);
	array_to_vector(v2, a2);

	std::sort(a1.begin(), a1.end(), TJSVariantCompare);
	std::sort(a2.begin(), a2.end(), TJSVariantCompare);
	r.reserve(a1.size() + a2.size());
	std::set_union(a1.begin(), a1.end(),
				   a2.begin(), a2.end(),
				   std::back_inserter(r),
				   TJSVariantCompare);

	tTJSVariant result = createArray();
	tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
	for (tjs_int i = 0; i < tjs_int(r.size()); i++) {
		(void)dst.PropSetByNum(0, i, &(r[i]), NULL);
	}

	return result;
}

//----------------------------------------------------------------------
// 碩集合
tTJSVariant intersectionSet(tTJSVariant v1, tTJSVariant v2)
{
	std::vector<tTJSVariant> a1, a2, r;

	array_to_vector(v1, a1);
	array_to_vector(v2, a2);

	std::sort(a1.begin(), a1.end(), TJSVariantCompare);
	std::sort(a2.begin(), a2.end(), TJSVariantCompare);
	r.reserve(std::min(a1.size(), a2.size()));
	std::set_intersection(a1.begin(), a1.end(),
						  a2.begin(), a2.end(),
						  std::back_inserter(r),
						  TJSVariantCompare);

	tTJSVariant result = createArray();
	tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
	for (tjs_int i = 0; i < tjs_int(r.size()); i++) {
		(void)dst.PropSetByNum(0, i, &(r[i]), NULL);
	}

	return result;
}

//----------------------------------------------------------------------
// 差集合
tTJSVariant differenceSet(tTJSVariant v1, tTJSVariant v2)
{
	std::vector<tTJSVariant> a1, a2, r;

	array_to_vector(v1, a1);
	array_to_vector(v2, a2);

	std::sort(a1.begin(), a1.end(), TJSVariantCompare);
	std::sort(a2.begin(), a2.end(), TJSVariantCompare);
	r.reserve(a1.size());
	std::set_difference(a1.begin(), a1.end(),
						a2.begin(), a2.end(),
						std::back_inserter(r),
						TJSVariantCompare);

	tTJSVariant result = createArray();
	tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
	for (tjs_int i = 0; i < tjs_int(r.size()); i++) {
		(void)dst.PropSetByNum(0, i, &(r[i]), NULL);
	}

	return result;
}

//----------------------------------------------------------------------
// 配列のスライス
tTJSVariant sliceArray(tTJSVariant v, tjs_int from, tjs_int size)
{
	tTJSVariantClosure &src = v.AsObjectClosureNoAddRef();
	tTJSVariant srcCount;
	(void)src.PropGet(0, L"count", &countHint, &srcCount, NULL);
	tjs_int count = srcCount;

	tTJSVariant result = createArray();
	tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();

	tTJSVariant value;
	for (tjs_int i = from; i < from + size; i++) {
		(void)src.PropGetByNum(TJS_IGNOREPROP, i, &value, NULL);
		(void)dst.PropSetByNum(0, i - from, &value, NULL);
	}

	return result;
}

//----------------------------------------------------------------------
// 配列の連結
tTJSVariant concatArray(tTJSVariant a1, tTJSVariant a2)
{
	tTJSVariant result = createArray();
	tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
	tjs_int dstCount = 0;

	{
		tTJSVariantClosure &src = a1.AsObjectClosureNoAddRef();
		tTJSVariant srcCount;
		(void)src.PropGet(0, L"count", &countHint, &srcCount, NULL);
		tjs_int count = srcCount;

		tTJSVariant value;
		for (tjs_int i = 0; i < count; i++) {
			(void)src.PropGetByNum(TJS_IGNOREPROP, i, &value, NULL);
			(void)dst.PropSetByNum(0, dstCount++, &value, NULL);
		}
	}
	{
		tTJSVariantClosure &src = a2.AsObjectClosureNoAddRef();
		tTJSVariant srcCount;
		(void)src.PropGet(0, L"count", &countHint, &srcCount, NULL);
		tjs_int count = srcCount;

		tTJSVariant value;
		for (tjs_int i = 0; i < count; i++) {
			(void)src.PropGetByNum(TJS_IGNOREPROP, i, &value, NULL);
			(void)dst.PropSetByNum(0, dstCount++, &value, NULL);
		}
	}

	return result;
}

//----------------------------------------------------------------------
// 全配列巡回
tjs_error TJS_INTF_METHOD eachArray(tTJSVariant *result,
									tjs_int numparams,
									tTJSVariant **param,
									iTJSDispatch2 *objthis) {
	if (numparams < 2) return TJS_E_BADPARAMCOUNT;
	tTJSVariantClosure &array = param[0]->AsObjectClosureNoAddRef();
	tTJSVariantClosure &func = param[1]->AsObjectClosureNoAddRef();

	tTJSVariant key, value;
	tTJSVariant **paramList = new tTJSVariant *[numparams];
	paramList[0] = &key;
	paramList[1] = &value;
	for (tjs_int i = 2; i < numparams; i++)
		paramList[i] = param[i];

	tTJSVariant arrayCount;
	(void)array.PropGet(0, L"count", &countHint, &arrayCount, NULL);
	tjs_int count = arrayCount;

	for (tjs_int i = 0; i < count; i++) {
		key = i;
		(void)array.PropGetByNum(TJS_IGNOREPROP, i, &value, NULL);
		(void)func.FuncCall(0, NULL, NULL, NULL, numparams, paramList, NULL);
	}

	delete[] paramList;

	return TJS_S_OK;
}

//----------------------------------------------------------------------
// 辞書を巡回するcaller
class DictIterateCaller : public tTJSDispatch
{
public:
	tTJSVariantClosure &dst;
	tTJSVariant **paramList;
	tjs_int paramCount;

	DictIterateCaller(tTJSVariantClosure &_dst,
					  tTJSVariant **_paramList,
					  tjs_int _paramCount)
		: dst(_dst)
		, paramList(_paramList)
		, paramCount(_paramCount) {
	}

	virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
											   tjs_uint32 flag,			// calling flag
											   const tjs_char * membername,// member name ( NULL for a default member )
											   tjs_uint32 *hint,			// hint for the member name (in/out)
											   tTJSVariant *result,		// result
											   tjs_int numparams,			// number of parameters
											   tTJSVariant **param,		// parameters
											   iTJSDispatch2 *objthis		// object as "this"
												) {
		if (numparams > 1) {
			if ((int)*param[1] != TJS_HIDDENMEMBER) {
				*paramList[0] = *param[0];
				*paramList[1] = *param[2];
				(void)dst.FuncCall(0, NULL, NULL, NULL, paramCount, paramList, NULL);
			}
		}
		if (result)
			*result = true;
		return TJS_S_OK;
	}
};

//----------------------------------------------------------------------
// 全辞書巡回
tjs_error TJS_INTF_METHOD eachDictionary(tTJSVariant *result,
										 tjs_int numparams,
										 tTJSVariant **param,
										 iTJSDispatch2 *objthis) {
	if (numparams < 2) return TJS_E_BADPARAMCOUNT;
	tTJSVariantClosure &dictionary = param[0]->AsObjectClosureNoAddRef();
	tTJSVariantClosure &func = param[1]->AsObjectClosureNoAddRef();

	tTJSVariant key, value;
	tTJSVariant **paramList = new tTJSVariant *[numparams];
	paramList[0] = &key;
	paramList[1] = &value;
	for (tjs_int i = 2; i < numparams; i++)
		paramList[i] = param[i];

	DictIterateCaller *caller = new DictIterateCaller(func, paramList, numparams);
	tTJSVariantClosure closure(caller);
	dictionary.EnumMembers(TJS_IGNOREPROP, &closure, NULL);
	caller->Release();

	delete[] paramList;

	return TJS_S_OK;
}

//----------------------------------------------------------------------
// プロパティ取得
bool getPropertyFromStyle(tTJSVariant style, tTJSVariant states, tTJSVariant key, tTJSVariant result)
{
	ncbPropAccessor styleObj(style);

	if (states.Type() != tvtVoid) {
		auto statesCount = countArray(states);
		ncbPropAccessor statesObj(states);
		for (tjs_uint i = 0; i < statesCount; i++) {
			ttstr state = statesObj.GetValue(i, ncbTypedefs::Tag<ttstr>());
			if (styleObj.HasValue(state.c_str())) {
				tTJSVariant nullObject;
				if (getPropertyFromStyle(styleObj.GetValue(state.c_str(), ncbTypedefs::Tag<tTJSVariant>()),
										 nullObject,
										 key,
										 result))
					return true;
			}
		}
	}

	if (key.Type() == tvtString) {
		ttstr keyString = key;
		if (keyString != L""
			&& styleObj.HasValue(keyString.c_str())) {
			ncbPropAccessor resultObj(result);
			resultObj.SetValue(tjs_int(0), styleObj.GetValue(keyString.c_str(), ncbTypedefs::Tag<tTJSVariant>()));
			return true;
		}
	} else {
		auto keysCount = countArray(key);
		ncbPropAccessor keysObj(key);
		for (tjs_uint i = 0; i < keysCount; i++) {
			ttstr keyString = keysObj.GetValue(i, ncbTypedefs::Tag<ttstr>());
			if (keyString != L""
				&& styleObj.HasValue(keyString.c_str())) {
				ncbPropAccessor resultObj(result);
				resultObj.SetValue(tjs_int(0), styleObj.GetValue(keyString.c_str(), ncbTypedefs::Tag<tTJSVariant>()));
				return true;
			}
		}
	}

	return false;
}

tTJSVariant getPropertyFromStyleChain(tTJSVariant styleChain, tTJSVariant states, tTJSVariant key, tTJSVariant defaultValue)
{
	tTJSVariant result = createArray();

	tjs_uint styleChainCount = countArray(styleChain);
	ncbPropAccessor styleChainObj(styleChain);
	for (tjs_uint i = 0; i < styleChainCount; i++) {
		tTJSVariant style = styleChainObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
		if (getPropertyFromStyle(style, states, key, result))
			return ncbPropAccessor(result).GetValue(tjs_int(0), ncbTypedefs::Tag<tTJSVariant>());
	}
	return defaultValue;
}

void extractDefinedProperties(tTJSVariant style, tTJSVariant defines, tTJSVariant defineKeys) {
	ncbPropAccessor styleObj(style);

	tTJSVariant styleKeys = dictionaryKeys(style);
	ncbPropAccessor styleKeysObj(styleKeys);
	auto styleKeysCount = countArray(styleKeys);

	for (tjs_uint i = 0; i < styleKeysCount; i++) {
		ttstr key = styleKeysObj.GetValue(i, ncbTypedefs::Tag<ttstr>());
		tTJSVariant member = styleObj.GetValue(key.c_str(), ncbTypedefs::Tag<tTJSVariant>());
		if (member.Type() == tvtObject) {
			tTJSVariantClosure &m = member.AsObjectClosureNoAddRef();
			if (m.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL) == TJS_S_TRUE)
				extractDefinedProperties(member, defines, defineKeys);
		}
	}

	ncbPropAccessor defineKeysObj(defineKeys);
	auto defineKeysCount = countArray(defineKeys);

	for (tjs_uint i = 0; i < defineKeysCount; i++) {
		ttstr key = defineKeysObj.GetValue(i, ncbTypedefs::Tag<ttstr>());
		if (! styleObj.HasValue(key.c_str()))
			continue;
		if (styleObj.GetValue(key.c_str(), ncbTypedefs::Tag<tTJSVariant>())
			.Type() == tvtVoid)
			continue;
		tjs_uint defineIndex = styleObj.getIntValue(key.c_str());
		ncbPropAccessor definesObj(defines);
		tTJSVariant define = definesObj.GetValue(key.c_str(), ncbTypedefs::Tag<tTJSVariant>());
		ncbPropAccessor defineObj(define);
		tTJSVariant props = defineObj.GetValue(defineIndex, ncbTypedefs::Tag<tTJSVariant>());
		ncbPropAccessor propsObj(props);
		tTJSVariant propKeys = dictionaryKeys(props);
		ncbPropAccessor propKeysObj(propKeys);
		auto propKeysCount = countArray(propKeys);
		for (tjs_uint j = 0; j < propKeysCount; j++) {
			ttstr propKey = propKeysObj.GetValue(j, ncbTypedefs::Tag<ttstr>());
			if (! styleObj.HasValue(propKey.c_str()))
				styleObj.SetValue(propKey.c_str(), propsObj.GetValue(propKey.c_str(), ncbTypedefs::Tag<tTJSVariant>()));
		}
	}
}

tTJSVariant extractStyleWithChain(tTJSVariant chain, tTJSVariant states, tTJSVariant definition)
{
	tTJSVariant style = createDictionary();
	ncbPropAccessor styleObj(style);

	ncbPropAccessor definitionObj(definition);
	tjs_uint definitionCount = countArray(definition);
	for (tjs_uint i = 0; i < definitionCount; i++) {
		tTJSVariant def = definitionObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
		ncbPropAccessor defObj(def);
		ttstr memberKey = defObj.GetValue(tjs_int(0), ncbTypedefs::Tag<ttstr>());
		tTJSVariant styleKey = defObj.GetValue(tjs_int(1), ncbTypedefs::Tag<tTJSVariant>());
		tTJSVariant defaultValue = defObj.GetValue(tjs_int(2), ncbTypedefs::Tag<tTJSVariant>());
		styleObj.SetValue(memberKey.c_str(), getPropertyFromStyleChain(chain, states, styleKey, defaultValue));
	}
	return style;
}


NCB_REGISTER_FUNCTION(equalStruct, equalStruct);
NCB_REGISTER_FUNCTION(equalStructNumericLoose, equalStructNumericLoose);
NCB_REGISTER_FUNCTION(dictionaryKeys, dictionaryKeys);
NCB_REGISTER_FUNCTION(dictionaryValues, dictionaryValues);
NCB_REGISTER_FUNCTION(arrayHash, arrayHash);
NCB_REGISTER_FUNCTION(unionDictionary, unionDictionary);
NCB_REGISTER_FUNCTION(unionSet, unionSet);
NCB_REGISTER_FUNCTION(intersectionSet, intersectionSet);
NCB_REGISTER_FUNCTION(differenceSet, differenceSet);
NCB_REGISTER_FUNCTION(sliceArray, sliceArray);
NCB_REGISTER_FUNCTION(concatArray, concatArray);
NCB_REGISTER_FUNCTION(eachArray, eachArray);
NCB_REGISTER_FUNCTION(eachDictionary, eachDictionary);
NCB_REGISTER_FUNCTION(getPropertyFromStyle, getPropertyFromStyle);
NCB_REGISTER_FUNCTION(getPropertyFromStyleChain, getPropertyFromStyleChain);
NCB_REGISTER_FUNCTION(extractDefinedProperties, extractDefinedProperties);
NCB_REGISTER_FUNCTION(extractStyleWithChain, extractStyleWithChain);
