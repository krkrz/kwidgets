
# KGridLayout

KGridLayout�́A�O���b�h��ɃE�B�W�F�b�g��z�u���郌�C�A�E�g�E�B�W�F�b�g�ł��B

## �e�N���X

**KGridLayout** -> [KLayout](KLayout.md) -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## �R���X�g���N�^
```KGridLayout(window, options = %[])```

## �X�^�C��
- **borderStyle**, **paddingStyle**
  - �X�^�C���w��Ɋ�Â��A�q�E�B�W�F�b�g�̎��͂Ƀ{�[�_�[���`�悳��܂��B

## ���\�b�h
- **add**(*x, y, child, colspan = 1, rowspan = 1*);
  - (*x*,*y*) �Ŏw�肵���O���b�h���W�Ɏq�E�B�W�F�b�g*child*��ǉ����܂��B
  
	*colspan*, *rowspan* ���w�肷�邱�ƂŁA���ꂼ�ꉡ�����A�c������
	�����}�X�̃O���b�h���܂�����z�u���w��ł��܂��B

- **remove**(*child, doInvalidate = true*);
  - �w��̎q�E�B�W�F�b�g*child*���O���b�h����폜���܂��B

	�f�t�H���g����ł͍폜����*child*�������I��invalidate����܂����A
	*doInvalidate*��false���w�肷���invalidate���ȗ����A
	remove����*child*���ė��p���邱�Ƃ��ł��܂��B�B
