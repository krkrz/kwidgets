#define NOMINMAX
#include <windows.h>

#include <vector>
#include <algorithm>
#include <iterator>
#include "ncbind.hpp"

//----------------------------------------------------------------------
// 宣言
extern tTJSVariant createDictionary(void);
extern tTJSVariant createArray(void);
extern tTJSVariant &getTempArray();
extern tTJSVariant &getTempArray2();
extern void releaseTempArray();
extern tTJSVariant dictionaryKeys(tTJSVariant dictionary);
extern bool intersectsArray(tTJSVariant a1, tTJSVariant a2);
extern tTJSVariant _unionDictionary(tTJSVariant v1, tTJSVariant v2, bool recursive);

//----------------------------------------------------------------------
// 定数
static const tjs_int K_STYLE_DEF_INDEX_PROPERTY_KEY     = 0;
static const tjs_int K_STYLE_DEF_INDEX_SEARCH_KEY       = 1;
static const tjs_int K_STYLE_DEF_INDEX_DEFAULT_VALUE    = 2;
static const tjs_int K_STYLE_DEF_INDEX_GLOBAL_RESOLVER  = 3;
static const tjs_int K_STYLE_DEF_INDEX_DEFAULT_PROPERTY = 4;
static const tjs_int K_STYLE_DEF_INDEX_LOCAL_RESOLVER   = 5;


//----------------------------------------------------------------------
// 変数
tjs_uint32 countHint, _styleHint, addHint, ancestorsHint, reverseHint, windowHint, styleRepositoryHint, _idStyleKeysHint, idHint, findHint, isIdHint, _classStyleKeysHint, isClassHint, _classWeakStyleKeysHint, isClassWeakHint, isAttachedToWindowHint, insertHint, removeHint, _styleCompHint, classNameHint, styleParentHint, _styleFragCacheHint, classTreesHint, styleStatesHint, eraseHint;

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
// スタイルレポジトリ参照
static tTJSVariant *sStyleRepository;

tTJSVariant &getStyleRepository() {
	if (! sStyleRepository) {
		sStyleRepository = new tTJSVariant();
		TVPExecuteExpression(L"styleRepository", sStyleRepository);
	}
	return (*sStyleRepository);
}

void releaseStyleRepository() {
	delete sStyleRepository;
}

tTJSVariant
resolveGlobalFunction(ttstr funcname, tTJSVariant value)
{
	ncbPropAccessor styleRepositoryObj(getStyleRepository());
	tTJSVariant result;
	styleRepositoryObj.FuncCall(0, funcname.c_str(), NULL, &result, value);
	return result;
}

NCB_PRE_UNREGIST_CALLBACK(releaseStyleRepository);


//----------------------------------------------------------------------
// プロパティ取得
bool getPropertyFromStyle(tTJSVariant style, tTJSVariant key, tTJSVariant result)
{
	ncbPropAccessor styleObj(style);

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

	if (states.Type() != tvtVoid) {
		auto statesCount = countArray(states);
		ncbPropAccessor statesObj(states);
		for (tjs_uint i = 0; i < statesCount; i++) {
			ttstr state = statesObj.GetValue(i, ncbTypedefs::Tag<ttstr>());
			for (tjs_uint j = 0; j < styleChainCount; j++) {
				tTJSVariant style = styleChainObj.GetValue(j, ncbTypedefs::Tag<tTJSVariant>());
				ncbPropAccessor styleObj(style);
				if (styleObj.HasValue(state.c_str())) {
					tTJSVariant stateStyle = styleObj.GetValue(state.c_str(), ncbTypedefs::Tag<tTJSVariant>());
					if (getPropertyFromStyle(stateStyle, key, result))
						return ncbPropAccessor(result).GetValue(tjs_int(0), ncbTypedefs::Tag<tTJSVariant>());
				}
			}
		}
	}

	for (tjs_uint i = 0; i < styleChainCount; i++) {
		tTJSVariant style = styleChainObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
		if (getPropertyFromStyle(style, key, result))
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

void applyAdditionalFunction(tTJSVariant widget, tTJSVariant style, tTJSVariant definition)
{
	ncbPropAccessor widgetObj(widget), styleObj(style), definitionObj(definition);

	tjs_uint definitionCount = countArray(definition);
	for (tjs_uint i = 0; i < definitionCount; i++) {
		tTJSVariant def = definitionObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
		ncbPropAccessor defObj(def);
		ttstr memberKey = defObj.GetValue(tjs_int(K_STYLE_DEF_INDEX_PROPERTY_KEY), ncbTypedefs::Tag<ttstr>());

		tTJSVariant memberValue = styleObj.GetValue(memberKey.c_str(), ncbTypedefs::Tag<tTJSVariant>());
		if (memberValue.Type() == tvtVoid) {
			tTJSVariant initialProperty = defObj.GetValue(tjs_int(K_STYLE_DEF_INDEX_DEFAULT_PROPERTY), ncbTypedefs::Tag<tTJSVariant>());
			if (initialProperty.Type() != tvtVoid) {
				ttstr initialKey = ttstr(initialProperty);
				tTJSVariant initialValue = widgetObj.GetValue(initialKey.c_str(), ncbTypedefs::Tag<tTJSVariant>());
				styleObj.SetValue(memberKey.c_str(), initialValue);
			}
		}
		tTJSVariant resolveFunction = defObj.GetValue(tjs_int(K_STYLE_DEF_INDEX_LOCAL_RESOLVER), ncbTypedefs::Tag<tTJSVariant>());
		if (resolveFunction.Type() == tvtVoid)
			continue;
		ttstr functionName = ttstr(resolveFunction);
		memberValue = styleObj.GetValue(memberKey.c_str(), ncbTypedefs::Tag<tTJSVariant>());
		tTJSVariant functionResult;
		(void)widgetObj.FuncCall(0, functionName.c_str(), NULL, &functionResult, memberValue, style);
		styleObj.SetValue(memberKey.c_str(), functionResult);
	}
}

tTJSVariant extractStyleChain(tTJSVariant self, bool includeSelfToChain, tTJSVariant classes) 
{
	ncbPropAccessor selfObj(self);

	tTJSVariant result = createArray();
	ncbPropAccessor resultObj(result);

	if (! includeSelfToChain) {
		tTJSVariant selfStyle = selfObj.GetValue(L"_style", ncbTypedefs::Tag<tTJSVariant>(), 0, &_styleHint);
		resultObj.FuncCall(0, L"add", &addHint, NULL, selfStyle);
	}

	tTJSVariant list;

	if (selfObj.GetValue(L"isAttachedToWindow", ncbTypedefs::Tag<tjs_int>(), 0, &isAttachedToWindowHint)) {
		list = selfObj.GetValue(L"ancestors", ncbTypedefs::Tag<tTJSVariant>(), 0, &ancestorsHint);
		ncbPropAccessor listObj(list);
		listObj.FuncCall(0, L"reverse", &reverseHint, NULL, NULL);
		listObj.FuncCall(0, L"add", &addHint, NULL, selfObj.GetValue(L"window", ncbTypedefs::Tag<tTJSVariant>(), 0, &windowHint));
		listObj.FuncCall(0, L"add", &addHint, NULL, selfObj.GetValue(L"styleRepository", ncbTypedefs::Tag<tTJSVariant>(), 0, &styleRepositoryHint));
	} else {
		list = createArray();
		ncbPropAccessor listObj(list);
		listObj.FuncCall(0, L"add", &addHint, NULL, selfObj.GetValue(L"styleRepository", ncbTypedefs::Tag<tTJSVariant>(), 0, &styleRepositoryHint));
	}

	tjs_uint idInsertionIndex, classInsertionIndex, classWeakInsertionIndex;
	idInsertionIndex = classInsertionIndex = classWeakInsertionIndex = countArray(result);

	ttstr _id = selfObj.GetValue(L"_id", ncbTypedefs::Tag<ttstr>(), 0, &idHint);
	ncbPropAccessor listObj(list);
	tjs_uint listCount = countArray(list);
	ncbPropAccessor classesObj(classes);
	tjs_uint classesCount = countArray(classes);

	for (tjs_uint i = 0; i < listCount; i++) {
		tTJSVariant widget = listObj.GetValue(tjs_int(i), ncbTypedefs::Tag<tTJSVariant>());
		ncbPropAccessor widgetObj(widget);
		tTJSVariant widgetStyle = widgetObj.GetValue(L"_style", ncbTypedefs::Tag<tTJSVariant>(), 0, &_styleHint);
		ncbPropAccessor widgetStyleObj(widgetStyle);
		tTJSVariant funcResult;

		if (! includeSelfToChain) {
			tTJSVariant _idStyleKeys = widgetObj.GetValue(L"_idStyleKeys", ncbTypedefs::Tag<tTJSVariant>(), 0, &_idStyleKeysHint);
			ncbPropAccessor(_idStyleKeys).FuncCall(0, L"find", &findHint, &funcResult, _id);
			if (tjs_int(funcResult) >= 0) {
				tTJSVariant isId = widgetStyleObj.GetValue(L"isId", ncbTypedefs::Tag<tTJSVariant>(), 0, &isIdHint);
				tTJSVariant idStyle = ncbPropAccessor(isId).GetValue(_id.c_str(), ncbTypedefs::Tag<tTJSVariant>());
				resultObj.FuncCall(0, L"insert", &insertHint, NULL, tjs_int(idInsertionIndex), idStyle);
				idInsertionIndex++;
				classInsertionIndex++;
				classWeakInsertionIndex++;
			}
		}
		tTJSVariant _classStyleKeys = widgetObj.GetValue(L"_classStyleKeys", ncbTypedefs::Tag<tTJSVariant>(), 0, &_classStyleKeysHint);
		if (intersectsArray(_classStyleKeys, classes)) {
			tTJSVariant isClass = widgetStyleObj.GetValue(L"isClass", ncbTypedefs::Tag<tTJSVariant>(), 0, &isClassHint);
			ncbPropAccessor isClassObj(isClass);
			for (tjs_uint j = 0; j < classesCount; j++) {
				ttstr klass = classesObj.GetValue(tjs_int(j), ncbTypedefs::Tag<ttstr>());
				if (isClassObj.HasValue(klass.c_str())) {
					tTJSVariant classStyle = isClassObj.GetValue(klass.c_str(), ncbTypedefs::Tag<tTJSVariant>());
					resultObj.FuncCall(0, L"insert", &insertHint, NULL, tjs_int(classInsertionIndex), classStyle);
					classInsertionIndex++;
					classWeakInsertionIndex++;
				}
			}
		}
		tTJSVariant _classWeakStyleKeys = widgetObj.GetValue(L"_classWeakStyleKeys", ncbTypedefs::Tag<tTJSVariant>(), 0, &_classWeakStyleKeysHint);
		if (intersectsArray(_classWeakStyleKeys, classes)) {
			tTJSVariant isClassWeak = widgetStyleObj.GetValue(L"isClassWeak", ncbTypedefs::Tag<tTJSVariant>(), 0, &isClassWeakHint);
			ncbPropAccessor isClassWeakObj(isClassWeak);
			for (tjs_uint j = 0; j < classesCount; j++) {
				ttstr klass = classesObj.GetValue(tjs_int(j), ncbTypedefs::Tag<ttstr>());
				if (isClassWeakObj.HasValue(klass.c_str())) {
					tTJSVariant classWeakStyle = isClassWeakObj.GetValue(klass.c_str(), ncbTypedefs::Tag<tTJSVariant>());
					resultObj.FuncCall(0, L"insert", &insertHint, NULL, tjs_int(classWeakInsertionIndex), classWeakStyle);
					classWeakInsertionIndex++;
				}
			}
		}
	}
	return result;
}

tTJSVariant mergeFrags(tTJSVariant frags)
{
	ncbPropAccessor fragsObj(frags);
	tTJSVariant voidVar;
	tTJSVariant result;
	fragsObj.FuncCall(0, L"remove", &removeHint, NULL, voidVar);
	tjs_int fragsCount = countArray(frags);
	if (fragsCount == 0)
		return result;
	result = fragsObj.GetValue(tjs_int(0), ncbTypedefs::Tag<tTJSVariant>());
	for (tjs_int i = 1; i < fragsCount; i++)
		result = _unionDictionary(result, fragsObj.GetValue(tjs_int(i), ncbTypedefs::Tag<tTJSVariant>()), true);
	return result;
}

tTJSVariant extractStyle(tTJSVariant widget, tTJSVariant definition)
{
	ncbPropAccessor widgetObj(widget);
	tTJSVariant styleComp = widgetObj.GetValue(L"_styleComp", ncbTypedefs::Tag<tTJSVariant>(), 0, &_styleCompHint);
	ncbPropAccessor styleCompObj(styleComp);

	ncbPropAccessor definitionObj(definition);
	tjs_int definitionCount = countArray(definition);

	tTJSVariant style = createDictionary();
	ncbPropAccessor styleObj(style);

	for (tjs_int i = 0; i < definitionCount; i++) {
		tTJSVariant def = definitionObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
		ncbPropAccessor defObj(def);
		ttstr key = defObj.GetValue(tjs_int(0), ncbTypedefs::Tag<ttstr>());
		if (styleCompObj.HasValue(key.c_str()))
			styleObj.SetValue(key.c_str(), styleCompObj.GetValue(key.c_str(), ncbTypedefs::Tag<tTJSVariant>()));
		else
			styleObj.SetValue(key.c_str(), defObj.GetValue(tjs_int(2), ncbTypedefs::Tag<tTJSVariant>()));

	}
	applyAdditionalFunction(widget, style, definition);

	return style;
}


tTJSVariant extractStyleFrag(tTJSVariant style, tTJSVariant definitionsSet, tTJSVariant state)
{
	tTJSVariant voidVar;

	if (state.Type() != tvtVoid) {
		auto stateString = ttstr(state);
		ncbPropAccessor styleObj(style);
		if (! styleObj.HasValue(stateString.c_str()))
			return voidVar;
		style = styleObj.GetValue(stateString.c_str(), ncbTypedefs::Tag<tTJSVariant>());
	}

	ncbPropAccessor styleObj(style);
	tTJSVariant result = createDictionary();
	ncbPropAccessor resultObj(result);
	bool modified = false;

	ncbPropAccessor definitionsSetObj(definitionsSet);
	tjs_int definitionsSetCount = countArray(definitionsSet);

	for (tjs_int i = 0; i < definitionsSetCount; i++) {
		tTJSVariant defs = definitionsSetObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
		ncbPropAccessor defsObj(defs);
		tjs_int defsCount = countArray(defs);
		for (tjs_int j = 0; j < defsCount; j++) {
			tTJSVariant def = defsObj.GetValue(j, ncbTypedefs::Tag<tTJSVariant>());
			ncbPropAccessor defObj(def);
			ttstr propKey = defObj.GetValue(tjs_int(K_STYLE_DEF_INDEX_PROPERTY_KEY), ncbTypedefs::Tag<ttstr>());
			tTJSVariant searchKey = defObj.GetValue(tjs_int(K_STYLE_DEF_INDEX_SEARCH_KEY), ncbTypedefs::Tag<tTJSVariant>());
			tTJSVariant resolver =  defObj.GetValue(tjs_int(K_STYLE_DEF_INDEX_GLOBAL_RESOLVER), ncbTypedefs::Tag<tTJSVariant>());
			if (searchKey.Type() == tvtObject) {
				ncbPropAccessor searchKeyObj(searchKey);
				tjs_int searchKeyCount = countArray(searchKey);
				for (tjs_int k = 0; k < searchKeyCount; k++) {
					ttstr key = searchKeyObj.GetValue(k, ncbTypedefs::Tag<ttstr>());
					if (styleObj.HasValue(key.c_str())) {
						modified = true;
						tTJSVariant value = styleObj.GetValue(key.c_str(), ncbTypedefs::Tag<tTJSVariant>());
						if (resolver.Type() != tvtVoid)
							value = resolveGlobalFunction(resolver, value);
						resultObj.SetValue(propKey.c_str(), value);
						break;
					}
				}
			} else {
				ttstr key = searchKey;
				if (styleObj.HasValue(key.c_str())) {
					modified = true;
					tTJSVariant value = styleObj.GetValue(key.c_str(), ncbTypedefs::Tag<tTJSVariant>());
						if (resolver.Type() != tvtVoid)
							value = resolveGlobalFunction(resolver, value);
					resultObj.SetValue(propKey.c_str(), value);
				}
			}
		}
	}

	if (modified)
		return result;
	else
		return voidVar;
}


tTJSVariant findStyleFragForId(tTJSVariant widget, ttstr key, ttstr id, tTJSVariant definitionsSet, tTJSVariant state) {
	ncbPropAccessor widgetObj(widget);
	tTJSVariant styleFragCache = widgetObj.GetValue(L"_styleFragCache", ncbTypedefs::Tag<tTJSVariant>(), 0, &_styleFragCacheHint);
	ncbPropAccessor styleFragCacheObj(styleFragCache);

	if (styleFragCacheObj.HasValue(key.c_str()))
		return styleFragCacheObj.GetValue(key.c_str(), ncbTypedefs::Tag<tTJSVariant>());
	tTJSVariant styleParent = widgetObj.GetValue(L"styleParent", ncbTypedefs::Tag<tTJSVariant>(), 0, &styleParentHint);
	tTJSVariant aboveFrag;
	if (styleParent.Type() != tvtVoid) 
		aboveFrag = findStyleFragForId(styleParent, key, id, definitionsSet, state);
	tTJSVariant frag = aboveFrag;
	auto style = widgetObj.GetValue(L"_style", ncbTypedefs::Tag<tTJSVariant>(), 0, &_styleHint);
	ncbPropAccessor styleObj(style);
	if (styleObj.HasValue(L"isId", &isIdHint)) {
		auto isId = styleObj.GetValue(L"isId", ncbTypedefs::Tag<tTJSVariant>(), 0, &isIdHint);
		ncbPropAccessor isIdObj(isId);
		if (isIdObj.HasValue(id.c_str())) {
			auto frags = getTempArray();
			ncbPropAccessor fragsObj(frags);
			fragsObj.SetValue(tjs_int(0), aboveFrag);
			fragsObj.SetValue(tjs_int(1), extractStyleFrag(isIdObj.GetValue(id.c_str(), ncbTypedefs::Tag<tTJSVariant>()), definitionsSet, state));
			frag = mergeFrags(frags);
		}
	}
	styleFragCacheObj.SetValue(key.c_str(), frag);
	return frag;
}

tTJSVariant getStyleFragForId(tTJSVariant widget, tTJSVariant definitionsSet, ttstr uniqKey, tTJSVariant state) {
	ncbPropAccessor widgetObj(widget);
	ttstr id = widgetObj.GetValue(L"id", ncbTypedefs::Tag<ttstr>(), 0, &idHint);
	if (id.IsEmpty())
		return tTJSVariant();
	ttstr className = widgetObj.GetValue(L"className", ncbTypedefs::Tag<ttstr>(), 0, &classNameHint);
	ttstr key = className + L"#" + uniqKey + L"#id#" + id;
	if (state.Type() == tvtString)
		key += L"#" + ttstr(state);
	tTJSVariant styleParent = widgetObj.GetValue(L"styleParent", ncbTypedefs::Tag<tTJSVariant>(), 0, &styleParentHint);
	return findStyleFragForId(styleParent, key, id, definitionsSet, state);
}


tTJSVariant findStyleFragForClass(tTJSVariant widget, ttstr key, tTJSVariant classes, tTJSVariant definitionsSet, tTJSVariant state)
{
	ncbPropAccessor widgetObj(widget);
	tTJSVariant styleFragCache = widgetObj.GetValue(L"_styleFragCache", ncbTypedefs::Tag<tTJSVariant>(), 0, &_styleFragCacheHint);
	ncbPropAccessor styleFragCacheObj(styleFragCache);

	if (styleFragCacheObj.HasValue(key.c_str()))
		return styleFragCacheObj.GetValue(key.c_str(), ncbTypedefs::Tag<tTJSVariant>());
	tTJSVariant styleParent = widgetObj.GetValue(L"styleParent", ncbTypedefs::Tag<tTJSVariant>(), 0, &styleParentHint);
	tTJSVariant aboveFrag;
	if (styleParent.Type() != tvtVoid) 
		aboveFrag = findStyleFragForClass(styleParent, key, classes, definitionsSet, state);
	tTJSVariant frags = getTempArray();
	ncbPropAccessor fragsObj(frags);
	fragsObj.SetValue(tjs_int(0), aboveFrag);
	auto style = widgetObj.GetValue(L"_style", ncbTypedefs::Tag<tTJSVariant>(), 0, &_styleHint);
	ncbPropAccessor styleObj(style);
	if (styleObj.HasValue(L"isClass", &isClassHint)) {
		tTJSVariant isClass = styleObj.GetValue(L"isClass", ncbTypedefs::Tag<tTJSVariant>(), 0, &isClassHint);
		ncbPropAccessor isClassObj(isClass);
		ncbPropAccessor classesObj(classes);
		tjs_int classesCount = countArray(classes);
		for (tjs_int i = 0; i < classesCount; i++) {
			auto klass = classesObj.GetValue(i, ncbTypedefs::Tag<ttstr>());
			if (isClassObj.HasValue(klass.c_str()))
				fragsObj.FuncCall(0, L"insert", &insertHint, NULL, tjs_int(1), extractStyleFrag(isClassObj.GetValue(klass.c_str(), ncbTypedefs::Tag<tTJSVariant>()), definitionsSet, state));
		}
	}
	tTJSVariant frag = mergeFrags(frags);
	styleFragCacheObj.SetValue(key.c_str(), frag);
	return frag;
}

tTJSVariant getStyleFragForClass(tTJSVariant widget, tTJSVariant definitionsSet, ttstr uniqKey, tTJSVariant state)
{
	ncbPropAccessor widgetObj(widget);
	auto classes = widgetObj.GetValue(L"classTrees", ncbTypedefs::Tag<tTJSVariant>(), 0, &classTreesHint);
	ttstr className = widgetObj.GetValue(L"className", ncbTypedefs::Tag<ttstr>(), 0, &classNameHint);
	ttstr key = className + L"#" + uniqKey + L"#class";
	if (state.Type() == tvtString)
		key += L"#" + ttstr(state);
	tTJSVariant styleParent = widgetObj.GetValue(L"styleParent", ncbTypedefs::Tag<tTJSVariant>(), 0, &styleParentHint);
	return findStyleFragForClass(styleParent, key, classes, definitionsSet, state);
}

tTJSVariant findStyleFragForClassWeak(tTJSVariant widget, ttstr key, tTJSVariant classes, tTJSVariant definitionsSet, tTJSVariant state)
{
	ncbPropAccessor widgetObj(widget);
	tTJSVariant styleFragCache = widgetObj.GetValue(L"_styleFragCache", ncbTypedefs::Tag<tTJSVariant>(), 0, &_styleFragCacheHint);
	ncbPropAccessor styleFragCacheObj(styleFragCache);

	if (styleFragCacheObj.HasValue(key.c_str()))
		return styleFragCacheObj.GetValue(key.c_str(), ncbTypedefs::Tag<tTJSVariant>());
	tTJSVariant styleParent = widgetObj.GetValue(L"styleParent", ncbTypedefs::Tag<tTJSVariant>(), 0, &styleParentHint);
	tTJSVariant aboveFrag;
	if (styleParent.Type() != tvtVoid) 
		aboveFrag = findStyleFragForClassWeak(styleParent, key, classes, definitionsSet, state);
	tTJSVariant frags = getTempArray();
	ncbPropAccessor fragsObj(frags);
	fragsObj.SetValue(tjs_int(0), aboveFrag);
	auto style = widgetObj.GetValue(L"_style", ncbTypedefs::Tag<tTJSVariant>(), 0, &_styleHint);
	ncbPropAccessor styleObj(style);
	if (styleObj.HasValue(L"isClassWeak", &isClassWeakHint)) {
		tTJSVariant isClass = styleObj.GetValue(L"isClassWeak", ncbTypedefs::Tag<tTJSVariant>(), 0, &isClassWeakHint);
		ncbPropAccessor isClassObj(isClass);
		ncbPropAccessor classesObj(classes);
		tjs_int classesCount = countArray(classes);
		for (tjs_int i = 0; i < classesCount; i++) {
			auto klass = classesObj.GetValue(i, ncbTypedefs::Tag<ttstr>());
			if (isClassObj.HasValue(klass.c_str()))
				fragsObj.FuncCall(0, L"insert", &insertHint, NULL, tjs_int(1), extractStyleFrag(isClassObj.GetValue(klass.c_str(), ncbTypedefs::Tag<tTJSVariant>()), definitionsSet, state));
		}
	}
	tTJSVariant frag = mergeFrags(frags);
	styleFragCacheObj.SetValue(key.c_str(), frag);
	return frag;
}

tTJSVariant getStyleFragForClassWeak(tTJSVariant widget, tTJSVariant definitionsSet, ttstr uniqKey, tTJSVariant state)
{
	ncbPropAccessor widgetObj(widget);
	auto classes = widgetObj.GetValue(L"classTrees", ncbTypedefs::Tag<tTJSVariant>(), 0, &classTreesHint);
	ttstr className = widgetObj.GetValue(L"className", ncbTypedefs::Tag<ttstr>(), 0, &classNameHint);
	ttstr key = className + L"#" + uniqKey + L"#classweak";
	if (state.Type() == tvtString)
		key += L"#" + ttstr(state);
	tTJSVariant styleParent = widgetObj.GetValue(L"styleParent", ncbTypedefs::Tag<tTJSVariant>(), 0, &styleParentHint);
	return findStyleFragForClassWeak(styleParent, key, classes, definitionsSet, state);
}

void updateStyleComp(tTJSVariant widget, tTJSVariant defsSet, ttstr uniqKey)
{
	ncbPropAccessor widgetObj(widget);
	auto styleStates = widgetObj.GetValue(L"styleStates", ncbTypedefs::Tag<tTJSVariant>(), 0, &styleStatesHint);

	ncbPropAccessor styleStatesObj(styleStates);
	tjs_int styleStatesCount = countArray(styleStates);

	auto style = widgetObj.GetValue(L"_style", ncbTypedefs::Tag<tTJSVariant>(), 0, &_styleHint);

	tTJSVariant frags = getTempArray2();
	ncbPropAccessor fragsObj(frags);

	for (tjs_int i = 0; i < styleStatesCount; i++) {
		auto state = styleStatesObj.GetValue(i, ncbTypedefs::Tag<tTJSVariant>());
		fragsObj.FuncCall(0, L"add", &addHint, 0, extractStyleFrag(style, defsSet, state));
		fragsObj.FuncCall(0, L"add", &addHint, 0, getStyleFragForId(widget, defsSet, uniqKey, state));
		fragsObj.FuncCall(0, L"add", &addHint, 0, getStyleFragForClass(widget, defsSet, uniqKey, state));
		fragsObj.FuncCall(0, L"add", &addHint, 0, getStyleFragForClassWeak(widget, defsSet, uniqKey, state));
	}
	tTJSVariant voidVar;
	fragsObj.FuncCall(0, L"add", &addHint, 0, extractStyleFrag(style, defsSet, voidVar));
	fragsObj.FuncCall(0, L"add", &addHint, 0, getStyleFragForId(widget, defsSet, uniqKey, voidVar));
	fragsObj.FuncCall(0, L"add", &addHint, 0, getStyleFragForClass(widget, defsSet, uniqKey, voidVar));
	fragsObj.FuncCall(0, L"add", &addHint, 0, getStyleFragForClassWeak(widget, defsSet, uniqKey, voidVar));

	fragsObj.FuncCall(0, L"reverse", &reverseHint, 0);

	tTJSVariant styleComp = mergeFrags(frags);
	if (styleComp.Type() == tvtVoid)
		styleComp = createDictionary();

	widgetObj.SetValue(L"_styleComp", styleComp, 0, &_styleCompHint);
}

void resolveStatePriority(tTJSVariant states, tTJSVariant statePriority)
{
	ncbPropAccessor statesObj(states);
	ncbPropAccessor statePriorityObj(statePriority);
	tjs_uint statePriorityCount = countArray(statePriority);
	tTJSVariant result;

	std::vector<ttstr> matched;

	for (tjs_uint i = 0; i < statePriorityCount; i++) {
		ttstr state = statePriorityObj.GetValue(i, ncbTypedefs::Tag<ttstr>());
		statesObj.FuncCall(0, L"find", &findHint, &result, state);
		if (tjs_int(result) >= 0)
			matched.push_back(state);
	}

	if (matched.size() <= 1)
		return;

	for (std::size_t i = 0; i < matched.size() - 1; i++) {
		auto priorState = matched[i];
		statesObj.FuncCall(0, L"find", &findHint, &result, priorState);
		tjs_int priorStateIndex = tjs_int(result);
		for (std::size_t j = i + 1; j < matched.size(); j++) {
			auto comparedState = matched[j];
			statesObj.FuncCall(0, L"find", &findHint, &result, comparedState);
			tjs_int comparedStateIndex = tjs_int(result);
			if (comparedStateIndex < priorStateIndex) {
				statesObj.FuncCall(0, L"insert", &insertHint, NULL, priorStateIndex + 1, comparedState);
				statesObj.FuncCall(0, L"erase", &eraseHint, NULL, comparedStateIndex);
			}
		}
	}
}


NCB_REGISTER_FUNCTION(getPropertyFromStyle, getPropertyFromStyle);
NCB_REGISTER_FUNCTION(getPropertyFromStyleChain, getPropertyFromStyleChain);
NCB_REGISTER_FUNCTION(extractDefinedProperties, extractDefinedProperties);
NCB_REGISTER_FUNCTION(extractStyleWithChain, extractStyleWithChain);
NCB_REGISTER_FUNCTION(applyAdditionalFunction, applyAdditionalFunction);
NCB_REGISTER_FUNCTION(extractStyleChain, extractStyleChain);
NCB_REGISTER_FUNCTION(mergeFrags, mergeFrags);
NCB_REGISTER_FUNCTION(extractStyle, extractStyle);
NCB_REGISTER_FUNCTION(extractStyleFrag, extractStyleFrag);
NCB_REGISTER_FUNCTION(findStyleFragForId, findStyleFragForId);
NCB_REGISTER_FUNCTION(getStyleFragForId, getStyleFragForId);
NCB_REGISTER_FUNCTION(findStyleFragForClass, findStyleFragForClass);
NCB_REGISTER_FUNCTION(getStyleFragForClass, getStyleFragForClass);
NCB_REGISTER_FUNCTION(findStyleFragForClassWeak, findStyleFragForClassWeak);
NCB_REGISTER_FUNCTION(getStyleFragForClassWeak, getStyleFragForClassWeak);
NCB_REGISTER_FUNCTION(updateStyleComp, updateStyleComp);
NCB_REGISTER_FUNCTION(resolveStatePriority, resolveStatePriority);
