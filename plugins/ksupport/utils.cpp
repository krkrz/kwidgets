//
// $Id$
//


#define NOMINMAX
#include <windows.h>

#include <vector>
#include <algorithm>
#include <iterator>
#include "ncbind.hpp"


//----------------------------------------------------------------------
// 各種ユーティリティ関数群
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// 変数
static tjs_uint32 countHint, clearHint, findHint;

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
	tTJSVariant result;
	TVPExecuteExpression(L"[]", &result);
	return result;
}

static tTJSVariant *sTempArray, *sTempArray2;

tTJSVariant &getTempArray()
{
	if (! sTempArray) {
		sTempArray = new tTJSVariant();
		*sTempArray = createArray();
	}
	ncbPropAccessor(*sTempArray).FuncCall(0, L"clear", &clearHint, NULL);

	return *sTempArray;
}

tTJSVariant &getTempArray2()
{
	if (! sTempArray2) {
		sTempArray2 = new tTJSVariant();
		*sTempArray2 = createArray();
	}
	ncbPropAccessor(*sTempArray2).FuncCall(0, L"clear", &clearHint, NULL);

	return *sTempArray2;
}

void releaseTempArray()
{
	delete sTempArray;
	delete sTempArray2;
}

NCB_PRE_UNREGIST_CALLBACK(releaseTempArray);

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
// 配列の交差を判定
bool
intersectsArray(tTJSVariant a1, tTJSVariant a2)
{
	tjs_uint a1Count = countArray(a1);
	tjs_uint a2Count = countArray(a2);
	if (a1Count > a2Count) {
		std::swap(a1, a2);
		std::swap(a1Count, a2Count);
	}
	ncbPropAccessor a1Obj(a1), a2Obj(a2);
	for (tjs_uint i = 0; i < a1Count; i++) {
		ttstr value = a1Obj.GetValue(tjs_int(i), ncbTypedefs::Tag<ttstr>());
		tTJSVariant findResult;
		a2Obj.FuncCall(0, L"find", &findHint, &findResult, value);
		if (tjs_int(findResult) >= 0)
			return true;
	}
	return false;
}


//----------------------------------------------------------------------
// 辞書の要素を全比較するCaller
class DictMemberEquvalentCaller : public tTJSDispatch
{
public:
	tTJSVariantClosure &target;
	bool match;

	DictMemberEquvalentCaller(tTJSVariantClosure &_target)
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
			DictMemberEquvalentCaller *caller = new DictMemberEquvalentCaller(o2);
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
class DictMemberEquvalentNumericLooseCaller : public tTJSDispatch
{
public:
	tTJSVariantClosure &target;
	bool match;

	DictMemberEquvalentNumericLooseCaller(tTJSVariantClosure &_target)
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
			DictMemberEquvalentNumericLooseCaller *caller = new DictMemberEquvalentNumericLooseCaller(o2);
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
// 構造体比較
tjs_int compareStruct(tTJSVariant v1, tTJSVariant v2)
{
	// タイプがオブジェクトなら特殊判定
	if (v1.Type() == tvtObject
		&& v2.Type() == tvtObject) {
		if (v1.AsObjectNoAddRef() == v2.AsObjectNoAddRef())
			return 0;

		tTJSVariantClosure &o1 = v1.AsObjectClosureNoAddRef();
		tTJSVariantClosure &o2 = v2.AsObjectClosureNoAddRef();

		// 関数どうしなら特別扱いで関数比較
		if (o1.IsInstanceOf(0, NULL, NULL, L"Function", NULL)== TJS_S_TRUE
			&& o2.IsInstanceOf(0, NULL, NULL, L"Function", NULL)== TJS_S_TRUE) {
			if (v1.LittlerThan(v2))
				return -1;
			else if (v2.LittlerThan(v1))
				return 1;
			else
				return 0;
		}

		// Arrayどうしなら比較できる項目を比較
		if (o1.IsInstanceOf(0, NULL, NULL, L"Array", NULL)== TJS_S_TRUE
			&& o2.IsInstanceOf(0, NULL, NULL, L"Array", NULL)== TJS_S_TRUE) {
			// 長さの短い方まで比較
			tTJSVariant o1Count, o2Count;
			(void)o1.PropGet(0, L"count", &countHint, &o1Count, NULL);
			(void)o2.PropGet(0, L"count", &countHint, &o2Count, NULL);
			tjs_int count = std::min(tjs_int(o1Count), tjs_int(o2Count));
			tTJSVariant o1Val, o2Val;
			for (tjs_int i = 0; i < count; i++) {
				(void)o1.PropGetByNum(TJS_IGNOREPROP, i, &o1Val, NULL);
				(void)o2.PropGetByNum(TJS_IGNOREPROP, i, &o2Val, NULL);
				tjs_int result = compareStruct(o1Val, o2Val);
				if (result != 0)
					return result;
			}
			// 比較できる項目まで一致した場合配列の長さで比較
			if (tjs_int(o1Count) < tjs_int(o2Count))
				return -1;
			else if (tjs_int(o2Count) < tjs_int(o1Count))
				return 1;
			else
				return 0;
		}

		// Dictionaryどうしなら全項目を比較
		if (o1.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL)== TJS_S_TRUE
			&& o2.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL)== TJS_S_TRUE) {
			// 両辞書のキーを統合して比較
			tTJSVariant o1Keys, o2Keys;
			o1Keys = dictionaryKeys(v1);
			o2Keys = dictionaryKeys(v2);
			tTJSVariant keys = unionSet(o1Keys, o2Keys);
			ncbPropAccessor keysObj(keys);
			tjs_int keysCount = countArray(keys);
			for (tjs_int i = 0; i < keysCount; i++) {
				ttstr key = keysObj.GetValue(i, ncbTypedefs::Tag<ttstr>());
				tTJSVariant o1Val, o2Val;
				auto o1Result = o1.PropGet(TJS_IGNOREPROP | TJS_MEMBERMUSTEXIST, key.c_str(), NULL, &o1Val, NULL);
				auto o2Result = o2.PropGet(TJS_IGNOREPROP | TJS_MEMBERMUSTEXIST, key.c_str(), NULL, &o2Val, NULL);
				if (o1Result != TJS_S_OK && o2Result == TJS_S_OK)
					return -1;
				else if (o2Result != TJS_S_OK && o1Result == TJS_S_OK)
					return 1;
				else {
					tjs_int result = compareStruct(o1Val, o2Val);
					if (result != 0)
						return result;
				}
			}
			// 全キーを比較して差がなければ一致
			return 0;
		}
	}

	if (v1.LittlerThan(v2))
		return -1;
	else if (v2.LittlerThan(v1))
		return 1;
	else
		return 0;
}

//----------------------------------------------------------------------
// 構造体複製
tTJSVariant duplicateStruct(tTJSVariant src);

	class DictMemberDuplicateCaller : public tTJSDispatch
{
public:
	tTJSVariantClosure &target;

	DictMemberDuplicateCaller(tTJSVariantClosure &_target)
		: target(_target) {
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
				tTJSVariant value = duplicateStruct(*param[2]);
				target.PropSet(TJS_MEMBERENSURE, key, NULL, &value, NULL);
			}
		}
		return TJS_S_OK;
	}
};


tTJSVariant duplicateStruct(tTJSVariant src)
{
	if (src.Type() != tvtObject)
		return src;

	auto &o1 = src.AsObjectClosureNoAddRef();
	if (o1.IsInstanceOf(0, NULL, NULL, L"Function", NULL) == TJS_S_TRUE)
		return src;

	if (o1.IsInstanceOf(0, NULL, NULL, L"Array", NULL) == TJS_S_TRUE) {
		tTJSVariant dst = createArray();
		auto &o2 = dst.AsObjectClosureNoAddRef();
		tTJSVariant val;
		(void)o1.PropGet(0, L"count", &countHint, &val, NULL);
		tjs_int count = tjs_int(val);
		for (tjs_int i = 0; i < count; i++) {
			(void)o1.PropGetByNum(TJS_IGNOREPROP, i, &val, NULL);
			val = duplicateStruct(val);
			(void)o2.PropSetByNum(0, i, &val, NULL);
		}
		return dst;
	}

	if (o1.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL) == TJS_S_TRUE) {
		tTJSVariant dst = createDictionary();
		auto keys = dictionaryKeys(src);
		auto &o2 = dst.AsObjectClosureNoAddRef();
		auto caller = new DictMemberDuplicateCaller(o2);
		auto closure = tTJSVariantClosure(caller);
		o1.EnumMembers(TJS_IGNOREPROP, &closure, NULL);
		caller->Release();
		return dst;
	}

	return src;
}

NCB_REGISTER_FUNCTION(equalStruct, equalStruct);
NCB_REGISTER_FUNCTION(equalStructNumericLoose, equalStructNumericLoose);
NCB_REGISTER_FUNCTION(compareStruct, compareStruct);
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
NCB_REGISTER_FUNCTION(duplicateStruct, duplicateStruct);
NCB_REGISTER_FUNCTION(dup, duplicateStruct);
