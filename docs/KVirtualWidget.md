# KVirtualWidget

KVirtualWidget ��
�X�N���[���̈�̓��A���ۂɉ�ʂɕ\������鋷���̈�݂̂Ƀ��C���̎��̂����蓖�Ă邱�ƂŁA
���Ȃ�����������ōL��ȗ̈�̕`����\�ɂ���d�g�݂ł��B

KScrollLayout �Ɋi�[���� KVirtualWidget �̕\���̈悪�X�V�����ƁA
�V���ɕ\�����ꂽ�̈�̕`����s������ onDraw() �t�b�N���Ăяo����܂��B

onDraw() �t�b�N���ł́A�ʏ�̋g���g����
[Layer](https://krkrz.github.io/docs/kirikiriz/j/contents/f_Layer.html)
�Ɠ��l�̕`��֐��Q���g���ă��C���ւ̕`������N�G�X�g���邱�Ƃ��ł��܂��B

���ۂɂ͂����̕`�惊�N�G�X�g�́A�\���̈�Ɠ����̃T�C�Y���������Ȃ�
�����I�� **canvas** ���C���ɓ]�����ď�������܂��B

## �e�N���X

**KVirtualWidget** -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## �R���X�g���N�^
```KVirtualWidget(window, options = %[])```

## �v���p�e�B
- Layer **canvas** (readonly)
  - ���ۂ̕`�悪�s����L�����o�X���C��
- **siteLeft** (int)
  - ���݂�canvas�́A�X�N���[���̈撆�ɐ�߂鍶�[���W
- **siteTop** (int)
  - ���݂�canvas�́A�X�N���[���̈撆�ɐ�߂��[���W
- **siteWidth** (int)
  - ���݂�canvas�́A�X�N���[���̈撆�ɐ�߂镝
- **siteWidth** (int)
  - ���݂�canvas�́A�X�N���[���̈撆�ɐ�߂鍂��
- **avoidSiteUpdateDrawing** (bool)
  - �̈�X�V���̍ĕ`����s�����ǂ���

	���̃v���p�e�B��true���w�肷��ƁA�ꎞ�I�ɃT�C�g�X�V���̍ĕ`�悪�}������܂��B
	�`�惊�N�G�X�g�̓L���b�V������Afalse���w�肵���u�Ԃɍĕ`�悪���s����܂��B

	�E�B�W�F�b�g�ɍĕ`��𔺂��傫�ȕύX��A�����ĉ�����ہA
	�X�V���I���܂ŕ`���}������Ƃ������ړI�Ŏg�p���܂��B

## ���\�b�h
- **redrawRect**(*left, top, width, height*);
  - �E�B�W�F�b�g���̎w�肵���̈�ɍĕ`�惊�N�G�X�g�𑗂�܂��B
- **redrawAll**();
  - �E�B�W�F�b�g�̑S��ɍĕ`�惊�N�G�X�g�𑗂�܂��B

## �t�b�N
- **onDraw(left, top, width, height);
  - canvas�ւ̍ĕ`�悪�K�v�ȃ^�C�~���O�ŌĂ΂�܂��B

	�̈�̃X�N���[���A�E�B�W�F�b�g�̃��T�C�Y�A�`�惊�N�G�X�g�̔��s�ȂǁA
	�l�X�ȗv���ōĕ`�悪�K�v�ɂȂ����^�C�~���O�ŌĂяo�����t�b�N�ł��B

	�ʏ��Layer�̍X�V�Ɠ��l�̎菇�ŁA�w��̗̈�֕`�揈�������s���Ă��������B

## �����ς݂̃f���Q�[�g�`��֐�
- **drawText**();
- **drawUIText**();
- **drawUITextInRange**();
- **drawTextArea**();
- **colorRect**();
- **copyRect**();
- **blendRect**();
- **fillRect**();
- **operateRect**();
- **operateStretch**();
- **piledCopy**();
- **tretchCopy**();
- **affineCopy**();
- **operateaffine**();
- **drawArc**();
- **drawPie**();
- **drawBezier**();
- **drawBeziers**();
- **drawClosedCurve**();
- **drawClosedCurve2**();
- **drawCurve**();
- **drawCurve2**();
- **drawCurve3**();
- **drawEllipse**();
- **drawLine**();
- **drawLines**();
- **drawPolygon**();
- **drawRectangle**();
- **drawRectangles**();
- **drawPathString**();

## �C�ӂ̕`��֐����f���Q�[�g�Ώۂɂ���
KVirtualWidget�Ƀr���g�C������Ă��Ȃ��`��֐���
�Ή�������ɂ́Akwidgets��mixin�@�\���g���܂��B

��Ƃ��āADLL�o�R��Layer�N���X�Ɉȉ��̂悤�Ȏd�l�̕`��֐����ǉ����ꂽ�ꍇ���l���܂��B

	function grayscaleRect(left, top, width, height, rate);
	// �w��̗̈��ω������w�肵�ăO���[�X�P�[��������֐�

�����Ƃ��āA�܂��ȉ��̂悤�ȃf���Q�[�g�N���X�����܂��B

	class MyVirtualWidgetDelegate
		function grasyaleRect(left, top, width, height, rate) {
			canvas.grayscaleRect(left - siteLeft, top - siteTop, width, height, rate);
		}
	}

**KVirtualWidget#grayscaleRect**() ���Ăяo���ꂽ�Ƃ��ɁA
���W������킷 *left*��*top*�̈������A
���E�̃X�N���[���ʂł��� *siteLeft*��*siteTop*���g���ĕ␳������ŁA
���ۂ̕`���ł���canvas�ɑ΂��ăf���Q�[�g����悤�ɂ��Ă��܂��B

���Ƃ́A�ȉ��̂悤�ɂ��Ă��̃f���Q�[�g�N���X��KVirtualWidget�Ƀ~�b�N�X�C������ΑΉ������ł��B

	mixin(KVirtualWidget, MyVirtualWidgetDelegate);


