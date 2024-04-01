//
// $Id$
//


#define NOMINMAX
#define _USE_MATH_DEFINES

#include <windows.h>

#include <cstdio>
#include <cmath>
#include <algorithm>
#include "ncbind.hpp"


//----------------------------------------------------------------------
// ユーティリティ関数
enum map_t
{
  MAP_CONST,
  MAP_X,
  MAP_Y,
};

enum color_space_t {
	COLOR_SPACE_TYPE_MASK = 0x00ffff,

	COLOR_SPACE_RGB = 0,
	COLOR_SPACE_SRGB,
	COLOR_SPACE_HSV,
	COLOR_SPACE_HSL,
	COLOR_SPACE_HCL,

	COLOR_SPACE_MODE_MASK = 0xff0000,
	COLOR_SPACE_MODE_NONCYCLIC = 0x010000,
};

struct rgb_t
{
  int b; // 0.0-255
  int g; // 0.0-255
  int r; // 0.0-255

	rgb_t(int _r, int _g, int _b) : b(_b), g(_g), r(_r) {}
	rgb_t() {}
};

struct argb_t
{
	rgb_t rgb;
	int a;
	argb_t (int _r, int _g, int _b, int _a) : rgb(_r, _g, _b), a(_a) {}
	argb_t () {}
	argb_t (uint32_t color)
		: rgb((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff)
		, a((color >> 24) & 0xff) {
	}
	uint32_t toUint32(void) const {
		return (a << 24) | (rgb.r << 16) | (rgb.g << 8) | rgb.b;
	}
};

struct color_tuple_t
{
	double v[3];
};

struct color_tuple_a_t
{
	color_tuple_t color;
	int a;
};

struct range_t
{
	double min, max, cyclic;

	range_t(double _min, double _max, bool _cyclic = false) : min(_min), max(_max), cyclic(_cyclic) {}
};

//----------------------------------------------------------------------
// カラーコンバージョン

class ColorSpaceConverter
{
public:
	virtual color_tuple_t FromRGB(const rgb_t &from) const = 0;
	virtual rgb_t ToRGB(const color_tuple_t &from) const = 0;
	virtual range_t GetComponentRange(int componentIndex) const = 0;

	color_tuple_t Lerp(const color_tuple_t &from, const color_tuple_t &to, double t) const {
		color_tuple_t result;
		for (int i = 0; i < 3; i++) {
			auto fromV = from.v[i];
			auto toV = to.v[i];
			range_t range = GetComponentRange(i);
			if (range.cyclic) {
				if (std::fabs(fromV - toV) > (range.max - range.min) / 2) {
					if (fromV > toV) 
						toV += (range.max - range.min);
					else
						fromV += (range.max - range.min);
				}
				result.v[i] = fmod((fromV * (1 - t) + toV * t), range.max - range.min) + range.min;
					} else {
				result.v[i] = fromV * (1 - t) + toV * t;
			}
		}
		return result;
	}

	color_tuple_a_t FromARGB(const argb_t &from) const {
		color_tuple_a_t to;
		to.color = FromRGB(from.rgb);
		to.a = from.a;
		return to;
	}

	argb_t ToARGB(const color_tuple_a_t &from) const {
		argb_t to;
		to.rgb = ToRGB(from.color);
		to.a = from.a;
		return to;
	}

	color_tuple_a_t Lerp(const color_tuple_a_t &from, const color_tuple_a_t &to, double t) const {
		color_tuple_a_t result;
		result.color = Lerp(from.color, to.color, t);
		result.a = from.a * (1 - t) + to.a * t;
		return result;
	}
};

class SRGBConverter : public ColorSpaceConverter
{
public:
	double b1(double v) const {
		return (v > 0.0031308 ? std::pow(v, (1 / 2.4)) * 269.025 - 14.025 : v * 3294.6); // srgb to linear rgb
	}

	double a1(double v) const {
		return  (v > 10.314724 ? std::pow(((v + 14.025) / 269.025), 2.4) : v / 3294.6); // linear rgb to srgb;
	}

	virtual color_tuple_t FromRGB(const rgb_t &from) const {
		color_tuple_t to;
		to.v[0] = a1(from.r) * 255;
		to.v[1] = a1(from.g) * 255;
		to.v[2] = a1(from.b) * 255;
		return to;
	}

	virtual rgb_t ToRGB(const color_tuple_t &from) const {
		rgb_t to;
		to.r = b1(from.v[0] / 255);
		to.g = b1(from.v[1] / 255);
		to.b = b1(from.v[2] / 255);
		return to;
	}

	virtual range_t GetComponentRange(int componentIndex) const {
		return range_t(0, 255);
	}

};

class RGBConverter : public ColorSpaceConverter
{
public:
	virtual color_tuple_t FromRGB(const rgb_t &from) const {
		color_tuple_t to;
		to.v[0] = from.r;
		to.v[1] = from.g;
		to.v[2] = from.b;
		return to;
	}

	virtual rgb_t ToRGB(const color_tuple_t &from) const {
		rgb_t to;
		to.r = from.v[0];
		to.g = from.v[1];
		to.b = from.v[2];
		return to;
	}

	virtual range_t GetComponentRange(int componentIndex) const {
		if (componentIndex > 3)
			TVPThrowExceptionMessage(L"tuple index out of range.");
		return range_t(0, 255);
	}
};

class HSVConverter : public ColorSpaceConverter
{
private:
	bool cyclic;

public:
	HSVConverter(int mode)
		: cyclic(! (mode & COLOR_SPACE_MODE_NONCYCLIC)) {
	}

	virtual color_tuple_t FromRGB(const rgb_t &from) const {
		color_tuple_t to;

		double r, g, b;
		double max_v, min_v;
		double h, s, v;

		r = from.r;
		g = from.g;
		b = from.b;
		max_v = std::max(r, std::max(g, b));
		min_v = std::min(r, std::min(g, b));
		if (max_v == 0) {
			to.v[0] = 0;
			to.v[1] = 0;
			to.v[2] = 0;
			return to;
		}
		if (max_v == min_v) {
			to.v[0] = 0;
			to.v[1] = 0;
			to.v[2] = max_v / 255;
			return to;
		}
		if (max_v == r)
			h = ((g - b) / (max_v - min_v)) * 60;
		else if (max_v == g)
			h = ((b - r) / (max_v - min_v)) * 60 + 120;
		else
			h = ((r - g) / (max_v - min_v)) * 60 + 240;
		if (h < 0)
			h += 360;
		s = (max_v - min_v) / max_v;
		v = max_v / 255;

		to.v[0] = h;
		to.v[1] = s * 100;
		to.v[2] = v * 100;

		return to;
	}

	virtual rgb_t ToRGB(const color_tuple_t &from) const {
		rgb_t to;
		double h, s, v;
		double p, q, t, f;
		int hi;
		h = fmod(from.v[0], 360);
		s = from.v[1] / 100;
		v = from.v[2] / 100;
		if (s == 0) {
			to.r = to.g = to.b = int(v * 255);
			return to;
		}
		if (h == 360)
			h = 0;
		hi = (int)std::floor(h) / 60 % 6;
		f = h / 60 - std::floor(h / 60.0f);
		p = v * (1 - s);
		q = v * (1 - f * s);
		t = v * (1 - (1 - f) * s);
		float r, g, b;
		switch (hi) {
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		case 5: r = v; g = p; b = q; break;
		}
		to.r = int(r * 255);
		to.g = int(g * 255);
		to.b = int(b * 255);
		return to;
	}

	virtual range_t GetComponentRange(int componentIndex) const {
		if (componentIndex > 3)
			TVPThrowExceptionMessage(L"tuple index out of range.");

		switch (componentIndex) {
		default: return range_t(0, 360, cyclic);
		case 1: return range_t(0, 100);
		case 2: return range_t(0, 100);
		}
	}
};

class HSLConverter : public HSVConverter
{
public:
	HSLConverter(int mode)
		: HSVConverter(mode) {
	}

	virtual color_tuple_t FromRGB(const rgb_t &from) const {
		color_tuple_t to_hsv = HSVConverter::FromRGB(from);

		double hsv_s = to_hsv.v[1] / 100;
		double hsv_v = to_hsv.v[2] / 100;

		double hsl_l = hsv_v - hsv_v * hsv_s / 2;
		double hsl_s = (hsl_l == 0 || hsl_l == 1) ? 0 : (hsv_v - hsl_l) / std::min(hsl_l, 1 - hsl_l);

		color_tuple_t to;
		to.v[0] = to_hsv.v[0];
		to.v[1] = hsl_s * 100;
		to.v[2] = hsl_l * 100;
		return to;
	}
	virtual rgb_t ToRGB(const color_tuple_t &from) const {
		double hsl_s = from.v[1] / 100;
		double hsl_l = from.v[2] / 100;

		double hsv_v = hsl_l + hsl_s * std::min(hsl_l, 1 - hsl_l);
		double hsv_s = (hsv_v == 0) ? 0 : 2 - 2 * hsl_l / hsv_v;

		color_tuple_t from_hsv;

		from_hsv.v[0] = from.v[0];
		from_hsv.v[1] = hsv_s * 100;
		from_hsv.v[2] = hsv_v * 100;

		return HSVConverter::ToRGB(from_hsv);
	}
};

class HCLConverter : public ColorSpaceConverter
{
private:
	bool cyclic;

	double rgb255(double v) const {
		return (v < 255 ? (v > 0 ? v : 0) : 255);
	}

	double b1(double v) const {
		return v * 255;
	}
	double b2(double v) const {
		return (v > 0.2068965 ? std::pow(v, 3) : (v - 4.0 / 29) * (108.0 / 841));
	}
	double a1(double v) const {
		return v / 255;
	}
	double a2(double v) const {
		return (v > 0.0088564 ? std::pow(v, (1.0 / 3)) : v / (108.0 / 841) + 4.0 / 29);
	}

public:
	HCLConverter(int mode)
		: cyclic(! (mode & COLOR_SPACE_MODE_NONCYCLIC)) {
	}

	virtual color_tuple_t FromRGB(const rgb_t &from) const {
		color_tuple_t to;
		auto r = a1(from.r);
		auto g = a1(from.g);
		auto b = a1(from.b);
		auto y = a2(r * 0.222488403 + g * 0.716873169 + b * 0.06060791);
		auto l = 500 * (a2(r * 0.452247074 + g * 0.399439023 + b * 0.148375274) - y);
		auto q = 200 * (y - a2(r * 0.016863605 + g * 0.117638439 + b * 0.865350722));
		auto h = std::atan2(q, l) * (180 / M_PI);
		to.v[0] = h < 0 ? h + 360 : h;
		to.v[1] = sqrt(l * l + q * q);
		to.v[2] = 116 * y - 16;
		return to;
	}
	virtual rgb_t ToRGB(const color_tuple_t &from) const {
		rgb_t to;
		auto h = from.v[0] * M_PI / 180;
		auto c = from.v[1];
		auto l = (from.v[2] + 16) / 116;
		auto y = b2(l);
		auto x = b2(l + (c / 500) * std::cos(h));
		auto z = b2(l - (c / 200) * std::sin(h));
        to.r = rgb255(b1(x * 3.021973625 - y * 1.617392459 - z * 0.404875592));
        to.g = rgb255(b1(x * -0.943766287 + y * 1.916279586 + z * 0.027607165));
        to.b = rgb255(b1(x * 0.069407491 - y * 0.22898585 + z * 1.159737864));
		return to;
	}

	virtual range_t GetComponentRange(int componentIndex) const {
		if (componentIndex > 3)
			TVPThrowExceptionMessage(L"tuple index out of range.");
		switch (componentIndex) {
		default: return range_t(0, 360, cyclic);
		case 1: return range_t(0, 130); 
		case 2: return range_t(0, 100);
		}
	}
};

ColorSpaceConverter *
GenerateColorSpaceConverter(int cs) {
	int type = cs & COLOR_SPACE_TYPE_MASK;
	int mode = cs & COLOR_SPACE_MODE_MASK;

	switch (type) {
	default: return new RGBConverter(); break;
	case COLOR_SPACE_SRGB: return new SRGBConverter(); break;
	case COLOR_SPACE_HSV: return new HSVConverter(mode); break;
	case COLOR_SPACE_HSL: return new HSLConverter(mode); break;
	case COLOR_SPACE_HCL: return new HCLConverter(mode); break;
	}
}

//----------------------------------------------------------------------
class ColorSpace
{
private:
	static color_tuple_a_t fromArray(tTJSVariant array) {
		if (array.Type() != tvtObject) {
			tjs_uint32 color = tjs_uint32(tjs_int(array));
			color_tuple_a_t result = { (color >> 16) & 0xff, (color >> 8) & 0xff,  color & 0xff, (color >> 24) & 0xff };
			return result;
		} else {
			ncbPropAccessor arrayObj(array);
			color_tuple_a_t result = {
				arrayObj.getRealValue(0),
				arrayObj.getRealValue(1),
				arrayObj.getRealValue(2),
				arrayObj.getRealValue(3),
			};
			return result;
		}
	}

	static tTJSVariant toArray(color_tuple_a_t color) {
		iTJSDispatch2 *obj = TJSCreateArrayObject();
		tTJSVariant result(obj, obj);
		obj->Release();
		ncbPropAccessor resultObj(obj);
		resultObj.SetValue(0, color.color.v[0]);
		resultObj.SetValue(1, color.color.v[1]);
		resultObj.SetValue(2, color.color.v[2]);
		resultObj.SetValue(3, color.a);
		return result;
	}

public:
	static tTJSVariant colorToArgbTuple(tjs_uint32 color) {
		color_tuple_a_t rgbTuple = { (color >> 16) & 0xff, (color >> 8) & 0xff,  color & 0xff, (color >> 24) & 0xff };
		return toArray(rgbTuple);
	}

	static tjs_uint32 argbTupleToColor(tTJSVariant argbTuple) {
		auto c = fromArray(argbTuple);
		return (int(c.color.v[0]) << 16) | (int(c.color.v[1]) << 8) | int(c.color.v[2]) | (int(c.a) << 24);
	}

	static tTJSVariant convertColorTuple(tjs_int fromCS, tjs_int toCS, tTJSVariant colorTuple) {
		auto fromConverter = ::GenerateColorSpaceConverter(static_cast<color_space_t>(fromCS));
		auto toConverter = ::GenerateColorSpaceConverter(static_cast<color_space_t>(toCS));
		auto toColor = toConverter->FromARGB(fromConverter->ToARGB(fromArray(colorTuple)));
		delete fromConverter;
		delete toConverter;
		return toArray(toColor);
	}

	static tTJSVariant getComponentRanges(tjs_int cs)
	{
		iTJSDispatch2 *obj = TJSCreateArrayObject();
		tTJSVariant result(obj, obj);
		obj->Release();
		ncbPropAccessor resultObj(result);

		auto converter = ::GenerateColorSpaceConverter(static_cast<color_space_t>(cs));

		for (tjs_int i = 0; i < 3; i++) {
			auto range = converter->GetComponentRange(i);
			iTJSDispatch2 *obj = TJSCreateDictionaryObject();
			tTJSVariant dict(obj, obj);
			obj->Release();
			ncbPropAccessor dictObj(dict);
			dictObj.SetValue(L"min", range.min);
			dictObj.SetValue(L"max", range.max);
			dictObj.SetValue(L"cyclic", range.cyclic);
			resultObj.SetValue(i, dict);
		}
		return result;
	}

	static tTJSVariant lerpColorTuple(tjs_int cs, tTJSVariant t0, tTJSVariant t1, tjs_real t) {
		auto from = fromArray(t0);
		auto to = fromArray(t1);
		auto converter = ::GenerateColorSpaceConverter(cs);
		auto result = converter->Lerp(from, to, t);
		delete converter;
		return toArray(result);
	}
};

NCB_REGISTER_CLASS(ColorSpace)
{
	NCB_METHOD(colorToArgbTuple);
	NCB_METHOD(argbTupleToColor);
	NCB_METHOD(getComponentRanges);
	NCB_METHOD(lerpColorTuple);
	NCB_METHOD(convertColorTuple);
};


//----------------------------------------------------------------------
class LayerSupport
{
private:
  iTJSDispatch2 *mObjthis; //< オブジェクト情報の参照

public:
  LayerSupport(iTJSDispatch2 *objthis) : mObjthis(objthis) {
  }

  void update(void) {
    mObjthis->FuncCall(0, TJS_W("update"), NULL, NULL, 0, NULL, mObjthis);
  }

  tjs_int wrap_mod(tjs_int num, int denom) const {
	  if (num >= 0)
		  return num % denom;
	  else
		  return (num % denom + denom) % denom;
  }

  void copyWrappedRect(tjs_int dleft, tjs_int dtop, tjs_int dwidth, tjs_int dheight,
                       tTJSVariant src, tjs_int sleft, tjs_int stop, tjs_int swidth, tjs_int sheight,
                       tjs_int shiftLeft, tjs_int shiftTop) {
    ncbPropAccessor dstObj(mObjthis), srcObj(src);
    tjs_int dright = dleft + dwidth, dbottom = dtop + dheight;
    dleft = std::max(dleft, dstObj.GetValue(L"clipLeft", ncbTypedefs::Tag<tjs_int>()));
    dtop = std::max(dtop, dstObj.GetValue(L"clipTop", ncbTypedefs::Tag<tjs_int>()));
    dright = std::min(dright, dstObj.GetValue(L"clipLeft", ncbTypedefs::Tag<tjs_int>()) + dstObj.GetValue(L"clipWidth", ncbTypedefs::Tag<tjs_int>()));
    dbottom = std::min(dbottom, dstObj.GetValue(L"clipTop", ncbTypedefs::Tag<tjs_int>()) + dstObj.GetValue(L"clipHeight", ncbTypedefs::Tag<tjs_int>()));
    dwidth = dright - dleft;
    dheight = dbottom - dtop;
    tjs_int dstPitch = dstObj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
    unsigned char *dstBuffer = reinterpret_cast<unsigned char*>(dstObj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int64>()));
    tjs_int srcPitch = srcObj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
    const unsigned char *srcBuffer = reinterpret_cast<const unsigned char*>(srcObj.GetValue(L"mainImageBuffer", ncbTypedefs::Tag<tjs_int64>()));
    for (tjs_int y = dtop; y < dbottom; y++) {
      unsigned char *dst = dstBuffer + dstPitch * y + dleft * 4;
      const unsigned char *srcBase = srcBuffer + (stop + wrap_mod(shiftTop + y,  sheight)) * srcPitch;
      for (tjs_int x = dleft; x < dright; x++) {
        const unsigned char *src = srcBase + (sleft + wrap_mod(shiftLeft + x, swidth)) * 4;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
      }
    }
    update();
  }

	void blendWrappedRect(tjs_int dleft, tjs_int dtop, tjs_int dwidth, tjs_int dheight,
							tTJSVariant src, tjs_int sleft, tjs_int stop, tjs_int swidth, tjs_int sheight,
							tjs_int shiftLeft, tjs_int shiftTop, tjs_int opacity) {
		ncbPropAccessor dstObj(mObjthis), srcObj(src);
		tjs_int dright = dleft + dwidth, dbottom = dtop + dheight;
		dleft = std::max(dleft, dstObj.GetValue(L"clipLeft", ncbTypedefs::Tag<tjs_int>()));
		dtop = std::max(dtop, dstObj.GetValue(L"clipTop", ncbTypedefs::Tag<tjs_int>()));
		dright = std::min(dright, dstObj.GetValue(L"clipLeft", ncbTypedefs::Tag<tjs_int>()) + dstObj.GetValue(L"clipWidth", ncbTypedefs::Tag<tjs_int>()));
		dbottom = std::min(dbottom, dstObj.GetValue(L"clipTop", ncbTypedefs::Tag<tjs_int>()) + dstObj.GetValue(L"clipHeight", ncbTypedefs::Tag<tjs_int>()));
		dwidth = dright - dleft;
		dheight = dbottom - dtop;
		tjs_int dstPitch = dstObj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
		unsigned char *dstBuffer = reinterpret_cast<unsigned char*>(dstObj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int64>()));
		tjs_int srcPitch = srcObj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
		const unsigned char *srcBuffer = reinterpret_cast<const unsigned char*>(srcObj.GetValue(L"mainImageBuffer", ncbTypedefs::Tag<tjs_int64>()));
		for (tjs_int y = dtop; y < dbottom; y++) {
			unsigned char *dst = dstBuffer + dstPitch * y + dleft * 4;
			const unsigned char *srcBase = srcBuffer + (stop + wrap_mod(shiftTop + y,  sheight)) * srcPitch;
			for (tjs_int x = dleft; x < dright; x++) {
				const unsigned char *src = srcBase + (sleft + wrap_mod(shiftLeft + x, swidth)) * 4;
				tjs_int srcA = src[3] * opacity / 255;
				tjs_int dstA = dst[3];
				tjs_int dstB = dst[0] * dstA / 255, dstG = dst[1] * dstA / 255, dstR = dst[2] * dstA / 255;
				tjs_int srcB = src[0] * srcA / 255, srcG = src[1] * srcA / 255, srcR = src[2] * srcA / 255;
				dstR = std::min((dstR * (255 - srcA) + srcR * 255) / 255, 255);
				dstG = std::min((dstG * (255 - srcA) + srcG * 255) / 255, 255);
				dstB = std::min((dstB * (255 - srcA) + srcB * 255) / 255, 255);
				dstA = std::min((dstA * (255 - srcA) + srcA * 255) / 255, 255);
				if (dstA == 0) {
					dstR = dstG = dstB = 0;
				} else {
					dstR = std::min(dstR * 255 / dstA, 255);
					dstG = std::min(dstG * 255 / dstA, 255);
					dstB = std::min(dstB * 255 / dstA, 255);
				}
				dst[0] = dstB, dst[1] = dstG, dst[2] = dstR, dst[3] = dstA;
				dst += 4;
				src += 4;
			}
		}
		update();
	}

  inline static tjs_uint blendColor(tjs_uint c0, tjs_uint c1, tjs_int x, tjs_int w) {
    return (c0 * (w - x - 1) + c1 * x) / (w - 1);
  }

	//----------------------------------------------------------------------
	// decode color arg
	static color_tuple_a_t decodeColorArg(tjs_int cs, tTJSVariant arg) {
		if (arg.Type() == tvtObject) {
			ncbPropAccessor argObj(arg);
			color_tuple_a_t result = {
				argObj.getRealValue(0),
				argObj.getRealValue(1), 
				argObj.getRealValue(2), 
				argObj.getRealValue(3)
			};
			return result;
		} else {
			auto converter = ::GenerateColorSpaceConverter(cs);
			auto result = converter->FromARGB(tjs_int(arg));
			delete converter;
			return result;
		}
	}

	//----------------------------------------------------------------------
	// clipped rectangle area handler
	template<typename T> static void handleRect(iTJSDispatch2 *objthis, tjs_int left, tjs_int top, tjs_int width, tjs_int height, T &handler) {
		ncbPropAccessor layerObj(objthis);

		tjs_int layerWidth, layerHeight, pitch;
		unsigned char *imageBuffer;
		layerWidth = layerObj.GetValue(L"width",  ncbTypedefs::Tag<tjs_int>());
		layerHeight = layerObj.GetValue(L"height",  ncbTypedefs::Tag<tjs_int>());
		pitch = layerObj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
		imageBuffer = reinterpret_cast<unsigned char*>(layerObj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int64>()));
		tjs_int clipLeft, clipTop, clipWidth, clipHeight;
		clipLeft = layerObj.GetValue(L"clipLeft", ncbTypedefs::Tag<tjs_int>());
		clipTop = layerObj.GetValue(L"clipTop", ncbTypedefs::Tag<tjs_int>());
		clipWidth = layerObj.GetValue(L"clipWidth", ncbTypedefs::Tag<tjs_int>());
		clipHeight = layerObj.GetValue(L"clipHeight", ncbTypedefs::Tag<tjs_int>());

		tjs_int fromX = std::max(clipLeft, left);
		tjs_int toX = std::min(clipLeft + clipWidth, left + width);
		tjs_int fromY = std::max(clipTop, top);
		tjs_int toY = std::min(clipTop + clipHeight, top + height);

		if (fromY >= toY
			|| fromX >= toX)
			return;

		handler.handle(imageBuffer, pitch, left, top, width, height, fromX, toX, fromY, toY);

		objthis->FuncCall(0, TJS_W("update"), NULL, NULL, 0, NULL, objthis);
	}

	//----------------------------------------------------------------------
	// fill gradient rect l to r
	class FillGradientRectLRHandler {
	private:
		color_tuple_a_t color0, color1;
		tjs_int cs;

	public:
		FillGradientRectLRHandler(color_tuple_a_t _c0, color_tuple_a_t _c1, tjs_int _cs)
			: color0(_c0), color1(_c1), cs(_cs) {}
		void handle(unsigned char *imageBuffer,
					tjs_int pitch,
					tjs_int left, tjs_int top, tjs_int width, tjs_int height,
					tjs_int fromX, tjs_int toX, tjs_int fromY, tjs_int toY) {
			auto converter = ::GenerateColorSpaceConverter(static_cast<color_space_t>(cs));

			uint32_t *p = reinterpret_cast<uint32_t*>(imageBuffer + fromY * pitch + fromX * 4);
			for (tjs_int x = fromX; x < toX; x++, p ++) {
				double t = 1.0 * (x - left) / (width - 1);
				*p = converter->ToARGB(converter->Lerp(color0, color1, t)).toUint32();;
			}

			const unsigned char *src = imageBuffer + fromY * pitch + fromX * 4;
			for (tjs_int y = fromY + 1; y < toY; y++) {
				unsigned char *dst = imageBuffer + y * pitch + fromX * 4;
				memcpy(dst, src, (toX - fromX) * 4);
			}

			delete converter;
		}
	};

	static tjs_error TJS_INTF_METHOD fillGradientRectLR(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 6
			|| numparams > 7) 
			return TJS_E_BADPARAMCOUNT;

		tjs_int left = tjs_int(*param[0]);
		tjs_int top = tjs_int(*param[1]);
		tjs_int width = tjs_int(*param[2]);
		tjs_int height = tjs_int(*param[3]);
		tjs_int colorspace = numparams <= 6 ? 0 : tjs_int(*param[6]);
		auto c0 = decodeColorArg(colorspace, *param[4]);
		auto c1 = decodeColorArg(colorspace, *param[5]);

		FillGradientRectLRHandler handler(c0, c1, colorspace);
		handleRect(objthis, left, top, width, height, handler);

		return TJS_S_OK;
	}

	//----------------------------------------------------------------------
	// fill gradient rect u to d
	class FillGradientRectUDHandler {
	private:
		color_tuple_a_t color0, color1;
		tjs_int cs;

	public:
		FillGradientRectUDHandler(color_tuple_a_t _c0, color_tuple_a_t _c1, tjs_int _cs)
			: color0(_c0), color1(_c1), cs(_cs) {}
		void handle(unsigned char *imageBuffer,
					tjs_int pitch,
					tjs_int left, tjs_int top, tjs_int width, tjs_int height,
					tjs_int fromX, tjs_int toX, tjs_int fromY, tjs_int toY) {
			auto converter = ::GenerateColorSpaceConverter(static_cast<color_space_t>(cs));

			for (tjs_int y = fromY; y < toY; y++) {
				auto t = 1.0 * (y - top) / (height - 1);
				auto c = converter->ToARGB(converter->Lerp(color0, color1, t)).toUint32();
				tjs_uint32 *dst = reinterpret_cast<tjs_uint32*>(imageBuffer + y * pitch + fromX * 4);
				for (tjs_int x = fromX; x < toX; x++) 
					*dst++ = c;
			}

			delete converter;
		}
	};

	static tjs_error TJS_INTF_METHOD fillGradientRectUD(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 6
			|| numparams > 7) 
			return TJS_E_BADPARAMCOUNT;

		tjs_int left = tjs_int(*param[0]);
		tjs_int top = tjs_int(*param[1]);
		tjs_int width = tjs_int(*param[2]);
		tjs_int height = tjs_int(*param[3]);
		tjs_int colorspace = numparams <= 6 ? 0 : tjs_int(*param[6]);
		auto c0 = decodeColorArg(colorspace, *param[4]);
		auto c1 = decodeColorArg(colorspace, *param[5]);

		FillGradientRectUDHandler handler(c0, c1, colorspace);
		handleRect(objthis, left, top, width, height, handler);

		return TJS_S_OK;
	}

	//----------------------------------------------------------------------
	// fill gradient rect corners
	class FillGradientRectCornersHandler {
	private:
		color_tuple_a_t color0, color1, color2, color3;
		tjs_int cs;

	public:
		FillGradientRectCornersHandler(color_tuple_a_t _c0, color_tuple_a_t _c1, color_tuple_a_t _c2, color_tuple_a_t _c3, tjs_int _cs)
			: color0(_c0), color1(_c1), color2(_c2), color3(_c3), cs(_cs) {}
		void handle(unsigned char *imageBuffer,
					tjs_int pitch,
					tjs_int left, tjs_int top, tjs_int width, tjs_int height,
					tjs_int fromX, tjs_int toX, tjs_int fromY, tjs_int toY) {
			auto converter = ::GenerateColorSpaceConverter(static_cast<color_space_t>(cs));

			for (tjs_int y = fromY; y < toY; y++) {
				auto yt = 1.0 * (y - top) / (height - 1);
				auto colorL = converter->Lerp(color0, color2, yt);
				auto colorR = converter->Lerp(color1, color3, yt);
				tjs_uint32 *dst = reinterpret_cast<tjs_uint32*>(imageBuffer + y * pitch + fromX * 4);
				for (tjs_int x = fromX; x < toX; x++) {
					auto xt = 1.0 * (x - left) / (width - 1);
					auto c = converter->ToARGB(converter->Lerp(colorL, colorR, xt)).toUint32();
					*dst++ = c;
				}
			}

			delete converter;
		}
	};

	static tjs_error TJS_INTF_METHOD fillGradientRectCorners(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 8
			|| numparams > 9) 
			return TJS_E_BADPARAMCOUNT;

		tjs_int left = tjs_int(*param[0]);
		tjs_int top = tjs_int(*param[1]);
		tjs_int width = tjs_int(*param[2]);
		tjs_int height = tjs_int(*param[3]);
		tjs_int colorspace = numparams <= 8 ? 0 : tjs_int(*param[8]);
		auto c0 = decodeColorArg(colorspace, *param[4]);
		auto c1 = decodeColorArg(colorspace, *param[5]);
		auto c2 = decodeColorArg(colorspace, *param[6]);
		auto c3 = decodeColorArg(colorspace, *param[7]);

		FillGradientRectCornersHandler handler(c0, c1, c2, c3, colorspace);
		handleRect(objthis, left, top, width, height, handler);

		return TJS_S_OK;
	}

	//----------------------------------------------------------------------
	// color gradient rect l to r
	class ColorGradientRectLRHandler {
	private:
		color_tuple_a_t color0, color1;
		tjs_int cs;

	public:
		ColorGradientRectLRHandler(color_tuple_a_t _c0, color_tuple_a_t _c1, tjs_int _cs)
			: color0(_c0), color1(_c1), cs(_cs) {}
		void handle(unsigned char *imageBuffer,
					tjs_int pitch,
					tjs_int left, tjs_int top, tjs_int width, tjs_int height,
					tjs_int fromX, tjs_int toX, tjs_int fromY, tjs_int toY) {
			auto converter = ::GenerateColorSpaceConverter(static_cast<color_space_t>(cs));

			unsigned char *buf = new unsigned char[(toX - fromX) * 4];
			uint32_t *p = reinterpret_cast<uint32_t*>(buf);
			for (tjs_int x = fromX; x < toX; x++, p ++) {
				double t = 1.0 * (x - left) / (width - 1);
				*p = converter->ToARGB(converter->Lerp(color0, color1, t)).toUint32();;
			}

			for (tjs_int y = fromY; y < toY; y++) {
				unsigned char *dst = imageBuffer + y * pitch + fromX * 4;
				TVPAlphaBlend((tjs_uint32*)dst, (const tjs_uint32*)buf, (toX - fromX));
			}

			delete[] buf;

			delete converter;
		}
	};


	static tjs_error TJS_INTF_METHOD colorGradientRectLR(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 6
			|| numparams > 7) 
			return TJS_E_BADPARAMCOUNT;

		tjs_int left = tjs_int(*param[0]);
		tjs_int top = tjs_int(*param[1]);
		tjs_int width = tjs_int(*param[2]);
		tjs_int height = tjs_int(*param[3]);
		tjs_int colorspace = numparams <= 6 ? 0 : tjs_int(*param[6]);
		auto c0 = decodeColorArg(colorspace, *param[4]);
		auto c1 = decodeColorArg(colorspace, *param[5]);

		ColorGradientRectLRHandler handler(c0, c1, colorspace);
		handleRect(objthis, left, top, width, height, handler);

		return TJS_S_OK;
	}


	//----------------------------------------------------------------------
	// color gradient rect u to d
	class ColorGradientRectUDHandler {
	private:
		color_tuple_a_t color0, color1;
		tjs_int cs;

	public:
		ColorGradientRectUDHandler(color_tuple_a_t _c0, color_tuple_a_t _c1, tjs_int _cs)
			: color0(_c0), color1(_c1), cs(_cs) {}
		void handle(unsigned char *imageBuffer,
					tjs_int pitch,
					tjs_int left, tjs_int top, tjs_int width, tjs_int height,
					tjs_int fromX, tjs_int toX, tjs_int fromY, tjs_int toY) {
			auto converter = ::GenerateColorSpaceConverter(static_cast<color_space_t>(cs));

			for (tjs_int y = fromY; y < toY; y++) {
				auto t = 1.0 * (y - top) / (height - 1);
				auto c = converter->ToARGB(converter->Lerp(color0, color1, t)).toUint32();
				tjs_uint32 *dst = reinterpret_cast<tjs_uint32*>(imageBuffer + y * pitch + fromX * 4);
				TVPConstColorAlphaBlend(dst, toX - fromX, c & 0xffffff, c >> 24);
			}

			delete converter;
		}
	};

	static tjs_error TJS_INTF_METHOD colorGradientRectUD(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 6
			|| numparams > 7) 
			return TJS_E_BADPARAMCOUNT;

		tjs_int left = tjs_int(*param[0]);
		tjs_int top = tjs_int(*param[1]);
		tjs_int width = tjs_int(*param[2]);
		tjs_int height = tjs_int(*param[3]);
		tjs_int colorspace = numparams <= 6 ? 0 : tjs_int(*param[6]);
		auto c0 = decodeColorArg(colorspace, *param[4]);
		auto c1 = decodeColorArg(colorspace, *param[5]);

		ColorGradientRectUDHandler handler(c0, c1, colorspace);
		handleRect(objthis, left, top, width, height, handler);

		return TJS_S_OK;
	}


	//----------------------------------------------------------------------
	// color gradient rect corners
	class ColorGradientRectCornersHandler {
	private:
		color_tuple_a_t color0, color1, color2, color3;
		tjs_int cs;

	public:
		ColorGradientRectCornersHandler(color_tuple_a_t _c0, color_tuple_a_t _c1, color_tuple_a_t _c2, color_tuple_a_t _c3, tjs_int _cs)
			: color0(_c0), color1(_c1), color2(_c2), color3(_c3), cs(_cs) {}
		void handle(unsigned char *imageBuffer,
					tjs_int pitch,
					tjs_int left, tjs_int top, tjs_int width, tjs_int height,
					tjs_int fromX, tjs_int toX, tjs_int fromY, tjs_int toY) {
			auto converter = ::GenerateColorSpaceConverter(static_cast<color_space_t>(cs));

			unsigned char *buf = new unsigned char[(toX - fromX) * 4];

			for (tjs_int y = fromY; y < toY; y++) {
				auto yt = 1.0 * (y - top) / (height - 1);
				auto colorL = converter->Lerp(color0, color2, yt);
				auto colorR = converter->Lerp(color1, color3, yt);
				uint32_t *p = reinterpret_cast<uint32_t*>(buf);
				for (tjs_int x = fromX; x < toX; x++) {
					auto xt = 1.0 * (x - left) / (width - 1);
					auto c = converter->ToARGB(converter->Lerp(colorL, colorR, xt)).toUint32();
					*p++ = c;
				}
				tjs_uint32 *dst = reinterpret_cast<tjs_uint32*>(imageBuffer + y * pitch + fromX * 4);
				TVPAlphaBlend((tjs_uint32*)dst, (const tjs_uint32*)buf, (toX - fromX));
			}

			delete[] buf;

			delete converter;
		}
	};

	static tjs_error TJS_INTF_METHOD colorGradientRectCorners(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 8
			|| numparams > 9) 
			return TJS_E_BADPARAMCOUNT;

		tjs_int left = tjs_int(*param[0]);
		tjs_int top = tjs_int(*param[1]);
		tjs_int width = tjs_int(*param[2]);
		tjs_int height = tjs_int(*param[3]);
		tjs_int colorspace = numparams <= 8 ? 0 : tjs_int(*param[8]);
		auto c0 = decodeColorArg(colorspace, *param[4]);
		auto c1 = decodeColorArg(colorspace, *param[5]);
		auto c2 = decodeColorArg(colorspace, *param[6]);
		auto c3 = decodeColorArg(colorspace, *param[7]);

		ColorGradientRectCornersHandler handler(c0, c1, c2, c3, colorspace);
		handleRect(objthis, left, top, width, height, handler);

		return TJS_S_OK;
	}

  tjs_uint32 fetchBuffer(unsigned char* buffer, tjs_int width, tjs_int height, tjs_int sx, tjs_int sy) {
    bool areaOver = false;
    if (sx < 0) { sx = 0; areaOver = true; }
    if (sy < 0) { sy = 0; areaOver = true; }
    if (sx >= width) { sx = width - 1; areaOver = true; }
    if (sy >= height) { sy = height - 1; areaOver = true; }
    tjs_uint32 color = ((tjs_uint32*)buffer)[sx + sy * width];
    if (areaOver)
      color &= 0x00FFFFFF;
    return color;
  }

  tjs_uint32 blendColor32(tjs_uint32 c0, tjs_uint32 c1, tjs_real rate) {
    tjs_uint a, r, g, b;
    tjs_uint a0, r0, g0, b0;
    tjs_uint a1, r1, g1, b1;
    a0 = (c0 >> 24) & 0xff, r0 = (c0 >> 16) & 0xff, g0 = (c0 >> 8) & 0xff, b0 = c0 & 0xff;
    a1 = (c1 >> 24) & 0xff, r1 = (c1 >> 16) & 0xff, g1 = (c1 >> 8) & 0xff, b1 = c1 & 0xff;
    a = int(a0 * (1 - rate) + a1 * rate + 0.5);
    r = int(r0 * (1 - rate) + r1 * rate + 0.5);
    g = int(g0 * (1 - rate) + g1 * rate + 0.5); 
    b = int(b0 * (1 - rate) + b1 * rate + 0.5);
    tjs_uint32 color = (a << 24) | (r << 16) | (g << 8) | b;
    return color;
  }
  
  tjs_uint32 samplingBuffer(unsigned char* buffer, tjs_int width, tjs_int height, tjs_real sx, tjs_real sy) {
    tjs_int ix = floor(sx), iy = floor(sy);
    tjs_real fx = sx - ix, fy = sy - iy;
    tjs_uint32 c0 = fetchBuffer(buffer, width, height, ix    , iy);
    tjs_uint32 c1 = fetchBuffer(buffer, width, height, ix + 1, iy);
    tjs_uint32 c2 = fetchBuffer(buffer, width, height, ix,     iy + 1);
    tjs_uint32 c3 = fetchBuffer(buffer, width, height, ix + 1, iy + 1);
    tjs_uint32 c01 = blendColor32(c0, c1, fx);
    tjs_uint32 c23 = blendColor32(c2, c3, fx);
    tjs_uint32 result = blendColor32(c01, c23, fy);
    return result;
  }
  
  void roundRect(tjs_int left, tjs_int top, tjs_int width, tjs_int height, tjs_int cx, tjs_int cy) {
    ncbPropAccessor layerObj(mObjthis);

    tjs_int layerWidth, layerHeight, pitch;
    unsigned char *imageBuffer;
    layerWidth = layerObj.GetValue(L"width",  ncbTypedefs::Tag<tjs_int>());
    layerHeight = layerObj.GetValue(L"height",  ncbTypedefs::Tag<tjs_int>());
    pitch = layerObj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
    imageBuffer = reinterpret_cast<unsigned char*>(layerObj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int64>()));
    tjs_int clipLeft, clipTop, clipWidth, clipHeight;
    
    auto srcBuffer = new unsigned char [ width * height * 4 ];
    
    for (tjs_int ux = 0; ux < width; ux++) 
      for (tjs_int uy = 0; uy < height; uy++) 
	memcpy(srcBuffer + (ux + uy * width) * 4, imageBuffer + (left + ux) * 4 + (top + uy) * pitch, 4);

    cx -= left;
    cy -= top;

    for (tjs_int ux = 0; ux < width; ux++) 
      for (tjs_int uy = 0; uy < height; uy++) {
	tjs_int dx = ux - cx, dy = uy - cy;
	if (dx == 0 || dy == 0)
	  continue;
	auto expandRate = sqrt(dx * dx + dy * dy) / std::max(std::abs(dx), std::abs(dy));
	auto sampleX = cx + dx * expandRate;
	auto sampleY = cy + dy * expandRate;
	tjs_uint32 color = samplingBuffer(srcBuffer, width, height, sampleX, sampleY);
	*(tjs_uint32*)(imageBuffer + (left + ux) * 4 + (top + uy) * pitch) = color;
      }

    delete[] srcBuffer;

    this->update();
  }
};

NCB_GET_INSTANCE_HOOK(LayerSupport)
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

NCB_ATTACH_CLASS_WITH_HOOK(LayerSupport, Layer) {
  NCB_METHOD(copyWrappedRect);
  NCB_METHOD(blendWrappedRect);
  NCB_METHOD_RAW_CALLBACK(fillGradientRectLR, LayerSupport::fillGradientRectLR, 0);
  NCB_METHOD_RAW_CALLBACK(fillGradientRectUD, LayerSupport::fillGradientRectUD, 0);
  NCB_METHOD_RAW_CALLBACK(fillGradientRectCorners, LayerSupport::fillGradientRectCorners, 0);
  NCB_METHOD_RAW_CALLBACK(colorGradientRectLR, LayerSupport::colorGradientRectLR, 0);
  NCB_METHOD_RAW_CALLBACK(colorGradientRectUD, LayerSupport::colorGradientRectUD, 0);
  NCB_METHOD_RAW_CALLBACK(colorGradientRectCorners, LayerSupport::colorGradientRectCorners, 0);
  NCB_METHOD(roundRect);
};

//----------------------------------------------------------------------
static  void setConstants(void)
{
  // 定数を登録
  TVPExecuteExpression(L"global.csRGB = 0");
  TVPExecuteExpression(L"global.csSRGB = 1");
  TVPExecuteExpression(L"global.csHSV = 2");
  TVPExecuteExpression(L"global.csHSL = 3");
  TVPExecuteExpression(L"global.csHCL = 4");
  TVPExecuteExpression(L"global.csNonCyclic = 0x010000");
}

NCB_PRE_REGIST_CALLBACK(setConstants);

static void TJS_USERENTRY tryDeleteConstants(void *data)
{
  // 定数を削除
  TVPExecuteScript(L"delete global[\"csRGB\"];");
  TVPExecuteScript(L"delete global[\"csSRGB\"];");
  TVPExecuteScript(L"delete global[\"csHSV\"];");
  TVPExecuteScript(L"delete global[\"csHSL\"];");
  TVPExecuteScript(L"delete global[\"csHCL\"];");
  TVPExecuteScript(L"delete global[\"csNonCyclic\"];");
}

static bool TJS_USERENTRY catchDeleteConstants(void *data, const tTVPExceptionDesc & desc) {
  return false;
}

static void deleteConstants(void)
{
	TVPDoTryBlock(&tryDeleteConstants, &catchDeleteConstants, NULL, NULL);
}

NCB_POST_UNREGIST_CALLBACK(deleteConstants);


