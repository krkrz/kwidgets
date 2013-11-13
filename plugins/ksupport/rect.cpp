//
// $Id$
//


#include <vector>
#include <algorithm>
#include "ncbind.hpp"


/**------------------------------
 * ��`
 ------------------------------*/
class KRect
{
public:
  tjs_real left, top, right, bottom;

  /**------------------------------
   * �R���X�g���N�^
   *
   * @param l ���[
   * @param t ��[
   * @param w ��
   * @param h ����
   ------------------------------*/
  KRect(tjs_real l, tjs_real t, tjs_real w, tjs_real h) {
    left = l;
    top = t;
    right = w + l;
    bottom = h + t;
  }

  /**------------------------------
   * ��`���L�����ǂ������肷��
   *
   * @return ��`���L�����ǂ���
   ------------------------------*/
  bool getIsValid(void) const  {
    return (left <= right && top <= bottom);
  }

  /*------------------------------
   * ��
   ------------------------------*/
  tjs_real getLeft(void) const {
    return left;
  }

  void setLeft(tjs_real v) {
    left = v;
  }

  /*------------------------------
   * �E
   ------------------------------*/
  tjs_real getRight(void) const {
    return right;
  }

  void setRight(tjs_real v) {
    right = v;
  }

  /*------------------------------
   * ��
   ------------------------------*/
  tjs_real getTop(void) const {
    return top;
  }

  void setTop(tjs_real v) {
    top = v;
  }
  
  /*------------------------------
   * ��
   ------------------------------*/
  tjs_real getBottom(void) const {
    return bottom;
  }

  void setBottom(tjs_real v) {
    bottom = v;
  }

  /**------------------------------
   * ��
   *
   * @return ��`�̕�
   ------------------------------*/
  tjs_real getWidth(void) const {
    return right - left;
  }

  void setWidth(tjs_real v) {
    right = left + v;
  }

  /**------------------------------
   * ����
   *
   * @return ��`�̍���
   ------------------------------*/
  tjs_real getHeight(void) const {
    return bottom - top;
  }
  void setHeight(tjs_real v) {
    bottom = top + v;
  }

  /**------------------------------
   * ��`�̕���
   *
   * @return �������g�𕡐�������`
   ------------------------------*/
  KRect *dup(void) const {
    return new KRect(left, top, getWidth(), getHeight());
  }

  /**------------------------------
   * ��`�̊��蓖��
   * 
   * @param rect ���蓖�Č�
   * 
   * rect�̓��e���������g�ɃR�s�[����
   ------------------------------*/
  void assign(const KRect *rect) {
    left = rect->left;
    top = rect->top;
    right = rect->right;
    bottom = rect->bottom;
  }

  /**------------------------------
   * ��������
   *
   * @param rect ��`
   * @return �������Ă��邩�H
   *
   * rect�Ǝ������g�ɏd�Ȃ肪���邩���肷��
   ------------------------------*/
  bool intersects(const KRect *rect) const {
    return ! (rect->right <= left
              || right <= rect->left
              || rect->bottom <= top
              || bottom <= rect->top);
  }

  /**------------------------------
   * ��ܔ���
   *
   * @param x X���W
   * @param y Y���W
   * @return ��܂��Ă��邩�H
   *
   * ��`�����W(X,Y)���܂��Ă��邩���肷��
   ------------------------------*/
  bool contains(tjs_real x, tjs_real y) const {
    return left <=x && x < right && top <= y && y <= bottom;
  }

  /**------------------------------
   * ��܉��Z
   *
   * @param rect ��`
   *
   * rect�Ǝ������g�̗������܂ލő�̋�`���A�������g�Ɋ��蓖�Ē���
   ------------------------------*/
  void include(const KRect *rect) {
    left = left < rect->left ? left : rect->left;
    top = top < rect->top ? top : rect->top;
    right = right > rect->right ? right : rect->right;
    bottom = bottom > rect->bottom ? bottom : rect->bottom;
  }

  /**------------------------------
   * �������Z
   *
   * @param rect ��`
   *
   * rect�Ǝ������g�̏d�Ȃ镔�����A�������g�Ɋ��蓖�Ē���
   ------------------------------*/
  void intersectWith(const KRect *rect) {
    left = left > rect->left ? left : rect->left;
    top = top > rect->top ? top : rect->top;
    right = right < rect->right ? right : rect->right;
    bottom = bottom < rect->bottom ? bottom : rect->bottom;
  }

  /**------------------------------
   * ��܉��Z
   *
   * @param rect ��`
   *
   * rect�Ǝ������g�̋��ʋ�`���A�������g�Ɋ��蓖�Ē���
   ------------------------------*/
  void unionWith(const KRect *rect) {
    left = left < rect->left ? left : rect->left;
    top = top < rect->top ? top : rect->top;
    right = right > rect->right ? right : rect->right;
    bottom = bottom > rect->bottom ? bottom : rect->bottom;
  }
};

NCB_REGISTER_CLASS(KRect)
{
  Constructor<tjs_real, tjs_real, tjs_real, tjs_real>(0);

  NCB_METHOD(dup);
  NCB_METHOD(assign);
  NCB_METHOD(intersects);
  NCB_METHOD(contains);
  NCB_METHOD(include);
  NCB_METHOD(intersectWith);
  NCB_METHOD(unionWith);

  NCB_PROPERTY_RO(isValid, getIsValid);
  NCB_PROPERTY(left, getLeft, setLeft);
  NCB_PROPERTY(right, getRight, setRight);
  NCB_PROPERTY(top, getTop, setTop);
  NCB_PROPERTY(bottom, getBottom, setBottom);
  NCB_PROPERTY(width, getWidth, setWidth);
  NCB_PROPERTY(height, getHeight, setHeight);
};


/**----------------------------------------------------------------------
 * �̈�
 ----------------------------------------------------------------------*/
class KRegion
{
public:
  std::vector<KRect> rectList;

  /**------------------------------
   * �R���X�g���N�^
   ------------------------------*/
  KRegion(void) {
  }

  /**------------------------------
   * �����Ɋ܂ދ�`�̐�
   ------------------------------*/
  tjs_uint getRectCount(void) const {
    return tjs_uint(rectList.size());
  }

  /**------------------------------
   * ��`�𒼐ڎQ�Ƃ���
   * 
   * @param index �C���f�b�N�X
   * @return ��`
   ------------------------------*/
  KRect *rectAt(tjs_uint index) {
    return rectList[index].dup();
  }

  /**------------------------------
   * �̈�͋󂩂ǂ������肷��
   *
   * @return �̈�͋󂩁H
   ------------------------------*/
  bool getEmpty(void) const {
    return rectList.empty();
  }

  /**------------------------------
   * �̈����ɂ���
   ------------------------------*/
  void clear(void) {
    rectList.clear();
  }

  /**------------------------------
   * �̈��ݒ肷��
   *
   * @param r KRect��������KRegion�I�u�W�F�N�g
   ------------------------------*/
  void assign(tTJSVariant r) {
    KRect *rect = ncbInstanceAdaptor<KRect>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (rect) {
      assignRect(rect);
      return;
    }
    KRegion *region = ncbInstanceAdaptor<KRegion>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (region) {
      assignRegion(region);
      return;
    }
    TVPThrowExceptionMessage(L"invalid argumen for function");
  }

  /**------------------------------
   * �̈���g������
   *
   * @param r KRect��������KRegion�I�u�W�F�N�g
   *
   * �w�肵���͈͂�̈�ɂ�������
   ------------------------------*/
  void include(tTJSVariant r) {
    KRect *rect = ncbInstanceAdaptor<KRect>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (rect) {
      includeRect(rect);
      return;
    }
    KRegion *region = ncbInstanceAdaptor<KRegion>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (region) {
      includeRegion(region);
      return;
    }
    TVPThrowExceptionMessage(L"invalid argumen for function");
  }

  /**------------------------------
   * �̈���팸����
   *
   * @param r KRect��������KRegion�I�u�W�F�N�g
   *
   * �w�肵���͈͂�̈悩���菜��
   ------------------------------*/
  void exclude(tTJSVariant r) {
    KRect *rect = ncbInstanceAdaptor<KRect>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (rect) {
      excludeRect(rect);
      return;
    }
    KRegion *region = ncbInstanceAdaptor<KRegion>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (region) {
      excludeRegion(region);
      return;
    }
    TVPThrowExceptionMessage(L"invalid argumen for function");
  }

  /**------------------------------
   * �������Z
   * 
   * @param r KRect��������KRegion�I�u�W�F�N�g
   *
   * �w�肵���͈͂����̈�̋��ʕ����������c���Ďc����폜����
   ------------------------------*/
  void intersectWith(tTJSVariant r) {
    KRect *rect = ncbInstanceAdaptor<KRect>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (rect) {
      intersectWithRect(rect);
      return;
    }
    KRegion *region = ncbInstanceAdaptor<KRegion>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (region) {
      intersectWithRegion(region);
      return;
    }
    TVPThrowExceptionMessage(L"invalid argumen for function");
  }

  /**------------------------------
   * ��������
   *
   * @param rect ��`
   * @return ��`�ƌ������Ă��邩�H
   *
   * ��`�ƌ������Ă��邩�ǂ����𔻒肷��
   ------------------------------*/
  bool intersects(const KRect * rect) const {
    for (tjs_uint i = 0; i < rectList.size(); i++)
      if (rectList[i].intersects(rect))
        return true;

    return false;
  }

  /**------------------------------
   * ��ܔ���
   *
   * @param x X���W
   * @param y Y���W
   * @return ���W���܂��Ă��邩�H
   *
   * ���W(X,Y)��̈���ɕ�܂��Ă��邩�ǂ����𔻒肷��
   ------------------------------*/
  bool contains(tjs_real x, tjs_real y) const {
    for (tjs_uint i = 0; i < rectList.size(); i++)
      if (rectList[i].contains(x, y))
        return true;

    return false;
  }

  /*----------------------------------------------------------------------
   * �ȉ��A��������
   ----------------------------------------------------------------------*/
  // ��`���蓖��
  void assignRect(const KRect *rect) {
    rectList.clear();
    rectList.push_back(*rect);
  }

  // �̈抄�蓖��
  void assignRegion(const KRegion *region) {
    rectList = region->rectList;
  }

  // ��`�ǉ�
  void includeRect(const KRect *rect) {
    KRegion r;
    r.assignRect(rect);
    r.excludeRegion(this);
    for (tjs_uint i = 0; i < r.rectList.size(); i++)
      rectList.push_back(r.rectList[i]);
  }

  // �̈�ǉ�
  void includeRegion(const KRegion *region) {
    if (region->getEmpty())
      return;
    
    KRegion r;
    r.assignRegion(region);
    r.excludeRegion(this);
    for (tjs_uint i = 0; i < r.rectList.size(); i++)
      rectList.push_back(r.rectList[i]);
  }

  // ��`����
  void excludeRect(const KRect *rect) {
    std::vector<KRect> newRectList;

    for (tjs_uint i = 0; i < rectList.size(); i++) {
      KRect &aRect = rectList[i];
      if (! aRect.intersects(rect)) {
        newRectList.push_back(aRect);
        continue;
      }
      if (rect->top > aRect.top) {
        newRectList.push_back(KRect(aRect.left, aRect.top, aRect.right - aRect.left, rect->top - aRect.top));
        aRect.top = rect->top;
      }
      if (rect->bottom < aRect.bottom) {
        newRectList.push_back(KRect(aRect.left, rect->bottom , aRect.right - aRect.left, aRect.bottom - rect->bottom));
        aRect.bottom = rect->bottom;
      }
      if (rect->left > aRect.left)
        newRectList.push_back(KRect(aRect.left, aRect.top, rect->left - aRect.left, aRect.bottom - aRect.top));
      if (rect->right < aRect.right)
        newRectList.push_back(KRect(rect->right, aRect.top, aRect.right - rect->right, aRect.bottom - aRect.top));
    }
    
    rectList.swap(newRectList);
  }

  // �̈揜��
  void excludeRegion(const KRegion *region) {
    for (tjs_uint32 i = 0; i < region->rectList.size(); i++)
      excludeRect(&(region->rectList[i]));
  }
  
  // ��`�Ƃ̌���
  void intersectWithRect(const KRect *rect) {
    std::vector<KRect> newRectList;

    for (tjs_uint i = 0; i < rectList.size(); i++) {
      KRect iRect(rectList[i]);
      iRect.intersectWith(rect);
      if (iRect.getIsValid())
        newRectList.push_back(iRect);
    }

    rectList.swap(newRectList);
  }

  // �̈�Ƃ̌���
  void intersectWithRegion(const KRegion *region) {
    std::vector<KRect> newRectList;

    for (tjs_uint i = 0; i < region->rectList.size(); i++)
      for (tjs_uint j = 0;j < rectList.size(); j++) {
        KRect iRect(region->rectList[i]);
        iRect.intersectWith(&rectList[j]);
        if (iRect.getIsValid())
          newRectList.push_back(iRect);
      }
    
    rectList.swap(newRectList);
  }
};

NCB_REGISTER_CLASS(KRegion)
{
  Constructor();

  NCB_METHOD(rectAt);
  NCB_METHOD(clear);
  NCB_METHOD(assign);
  NCB_METHOD(include);
  NCB_METHOD(exclude);
  NCB_METHOD(intersectWith);
  NCB_METHOD(intersects);
  NCB_METHOD(contains);

  NCB_PROPERTY_RO(rectCount, getRectCount);
  NCB_PROPERTY_RO(empty, getEmpty);
};

