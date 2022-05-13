# KWidget

KWidget�͂��ׂẴE�B�W�F�b�g�̊���ƂȂ�x�[�X�N���X�B

�g���g����Layer �N���X�̌p���N���X�Ȃ̂ŁA
Layer���Ή����Ă���`��֐��͂��ׂČĂяo����B

## �e�N���X

**KWidget** -> [Layer](https://krkrz.github.io/docs/kirikiriz/j/contents/f_Layer.html)

## �R���X�g���N�^
```KWidget(window, options = %[])```
### �I�v�V��������(�^: �����l)
- **name** (string: "")
  - ���O

	UI�c���[��̃E�B�W�F�b�g�͎w�肵�����O�Ŏ��ʉ\�ɂȂ�܂��B

- **id** (string: "")
  - ID

	 ID��ݒ肷�邱�ƂőΉ���������̃X�^�C�����w��\�ɂȂ�܂��B

- **style** (dictionary: %[])
  - �X�^�C��

	 �E�B�W�F�b�g�̌����ڂ��`����X�^�C�����w�肵�܂��B

- **status** (string: "")
  - �X�e�[�^�X

	�X�e�[�^�X���w�肷��ƁA���̃E�B�W�F�b�g�̃}�E�X�z�o�[����
	[KStatusBar](KStatusBar.md) �ɑΉ������X�e�[�^�X�e�L�X�g��
	�\�������悤�ɂȂ�܂��B

- **hint** (string: "")
  - �q���g�e�L�X�g

	�q���g���w�肷��ƁA���̃E�B�W�F�b�g�̃}�E�X�z�o�[����
	�|�b�v�A�b�v�E�B���h�E�Ńq���g�e�L�X�g���\�������悤�ɂȂ�܂��B

- **isAvoidDictionaryRegistration** (bool: false)
  - �i���������ւ̓o�^���������

## �v���p�e�B
- **id** (string)
  - ID
- **style** (dictionary)
  - �X�^�C��
- **status** (string)
  - �X�e�[�^�X
- **hint** (string)
  - �q���g�e�L�X�g
- **isAvoidDictionaryRegistration** (bool)
  - �i���������ւ̓o�^���������
- **ancestors** (Array)
  - �������g�̑c��E�B�W�F�b�g�̈ꗗ
- **descendants** (Array)
  - �������g�̎q���E�B�W�F�b�g�̈ꗗ
- **minWidth** (int)
  - �E�B�W�F�b�g�̍ŏ���
- **maxWidth** (int)
  - �E�B�W�F�b�g�̍ő啝
- **minHeight** (int)
  - �E�B�W�F�b�g�̍ŏ�����
- **maxHeight** (int)
  - �E�B�W�F�b�g�̍ő卂��
- **widgetStyle** (readonly dictionary)
  - �E�B�W�F�b�g�ŗL�̃X�^�C��  
	KWidget�̕W���X�^�C���Ƃ͕ʂɁA�p���N���X�ŃE�B�W�F�b�g�ŗL�̃X�^�C����`���i�[���邽�߂̎����B
- **fontStyle** (readonly dictionary)
  - �X�^�C���`�F�[�����璊�o���ꂽ�E�B�W�F�b�g�̌��݂̃t�H���g�X�^�C��  
  - %[  
	**face** (string) �t�F�C�X  
	**height** (int) �n�C�g  
	**color** (int) �J���[(0xRRGGBB)  
	**antiAlias** (bool) �A���`�G���A�X�̗L��  
	**outlline** (bool) �A�E�g���C���̗L��  
	**outlineColor** (int) �A�E�g���C���J���[(0xRRGGBB)  
	**bold** (bool) �{�[���h�̗L��  
	**italic** (bool) �C�^���b�N�̗L��  
	**strikeout** (bool) �X�g���C�N�A�E�g�̗L��  
	**underline** (bool) �A���_�[���C���̗L��  
	]

- **borderStyle** (readonly dictionary)
  - �X�^�C���`�F�[�����璊�o���ꂽ�E�B�W�F�b�g�̌��݂̃{�[�_�[�X�^�C��
  - %[  
	**backgroundColor** (int) �w�i�J���[(0xAARRGGBB)  
	**width{Left|Right|Top|Bottom}** (int) ��/�E/��/����  
	**color{Left|Right|Top|Bottom}** (int) ��/�E/��/���J���[(0xRRGGBB)  
	**style{Left|Right|Top|Bottom}** (int) ��/�E/��/���X�^�C��  
	**radius{LeftTop|RightTop|LeftBottom|RightBottom}** (int) ����/�E��/����/�E�����a  
	]

- **locateStyle** (readonly dictionary)
  - �X�^�C���`�F�[�����璊�o���ꂽ�E�B�W�F�b�g�̌��݂̃��P�[�g�X�^�C��
  - %[  
	**margin{Left|Right|Top|Bottom}** (int) �E�B�W�F�b�g�̍�/�E/��/���ɑ}�������󂫃X�y�[�X�̃T�C�Y  
	**alignHorizontal** (int) ���ɋ󂫃X�y�[�X������ۂɃE�B�W�F�b�g���A���C�����g�������  
	**alignVertical** (int) �c�ɋ󂫃X�y�[�X������ۂɃE�B�W�F�b�g���A���C�����g�������  
	]
- **marginLeft** (int)
  - locateStyle.marginLeft �̃V���[�g�J�b�g
- **marginRight** (int)
  - locateStyle.marginRight �̃V���[�g�J�b�g
- **marginTop** (int)
  - locateStyle.marginTop �̃V���[�g�J�b�g
- **marginBottom** (int)
  - locateStyle.marginBottom �̃V���[�g�J�b�g
- **alignHorizontal** (int)
  - locateStyle.alignHorizontal �̃V���[�g�J�b�g
- **layoutStyle** (readonly dictionary)
  - �X�^�C���`�F�[�����璊�o���ꂽ�E�B�W�F�b�g�̌��݂̃��C�A�E�g�X�^�C��
  - %[  
	**spaceHorizontal** (int) �q�E�B�W�F�b�g�����ɕ��ׂĔz�u����ۑ}�������󂫃X�y�[�X�̃T�C�Y  
	**spaceVertidcal** (int) �q�E�B�W�F�b�g���c�ɕ��ׂĔz�u����ۑ}�������󂫃X�y�[�X�̃T�C�Y  
	**padding{Left|Right|Top|Bottom>** (int) �q�E�B�W�F�b�g�̔z�u���ɍ�/�E/��/�����ɊJ����X�y�[�X�̃T�C�Y  
	]
- **spaceHorizontal** (int)
  - layoutStyle.spaceHorizontal �̃V���[�g�J�b�g
- **spaceVertical** (int)
  - layoutStyle.spaceVertical �̃V���[�g�J�b�g
- **paddingLeft** (int)
  - layoutStyle.paddingLeft �̃V���[�g�J�b�g
- **paddingRight** (int)
  - layoutStyle.paddingRight �̃V���[�g�J�b�g
- **paddingTop** (int)
  - layoutStyle.paddingTop �̃V���[�g�J�b�g
- **paddingBottom** (int)
  - layoutStyle.paddingBottom �̃V���[�g�J�b�g

- **borderPaddingLeft** (int)
  - layoutStyle.paddingLeft + borderStyle.widthLeft  
  �{�[�_�[�ƃp�f�B���O�̍����T�C�Y�̍��v�B  
  �q�E�B�W�F�b�g��z�u����ۂ̍����̗]���̊
- **borderPaddingRight** (int)
  - layoutStyle.paddingRight + borderStyle.widthRight  
  �{�[�_�[�ƃp�f�B���O�̉E���T�C�Y�̍��v�B  
  �q�E�B�W�F�b�g��z�u����ۂ̉E���̗]���̊
- **borderPaddingTop** (int)
  - layoutStyle.paddingTop + borderStyle.widthTop  
  �{�[�_�[�ƃp�f�B���O�̏㑤�T�C�Y�̍��v�B  
  �q�E�B�W�F�b�g��z�u����ۂ̏㑤�̗]���̊
- **borderPaddingBottom** (int)
  - layoutStyle.paddingBottom + borderStyle.widthBottom  
  �{�[�_�[�ƃp�f�B���O�̉����T�C�Y�̍��v�B  
  �q�E�B�W�F�b�g��z�u����ۂ̉����̗]���̊
- **borderPaddingWidth** (int)
  - borderPaddingLeft + borderPaddingRight;
- **borderPaddingHeight** (int)
  - borderPaddingTop + borderPaddingBottom;



## ���\�b�h
- var **getOption**(*key, defaultValue = void*);
  - �I�v�V�����������擾����B  
	�R���X�g���N�^�Ŏw�肳�ꂽ�I�v�V������������A
	�����*key*�ɑΉ������l���擾����B
	�Ή�����*key*�����݂��Ȃ����ɂ́A
	*defaultValue*���Ԃ��Ă���B

- var **getStyleProperty**(*key, defaultValue = void*);
  - �X�^�C���̃v���p�e�B���擾����B  
	���݂̃X�^�C���`�F�[������A
	*key*�ɑΉ������v���p�e�B���������Ă��̒l���擾����B
	*key*�ɑΉ�����v���p�e�B����`����Ă��Ȃ��ꍇ�ɂ�
	*defaultValue*���Ԃ��Ă���B

- object **find**(*name*);
  - �E�B�W�F�b�g�c���[�������̖��O�����E�B�W�F�b�g���������܂��B
- bool **isAncestorOf**(*widget*);
  - �w���*widget*�������̐�c�ł��邩���肷��B
- bool **isDescendantOf**(*widget*);
  - �w���*widget*�������̐�c�ł��邩���肷��B
- **setMinSize**(*minWidth, minHeight*);
  - �E�B�W�F�b�g�̍ŏ��T�C�Y��ݒ肷��
- **setMaxSize**(*maxWidth, maxHeight*);
  - �E�B�W�F�b�g�̍ő�T�C�Y��ݒ肷��
- **setMinMaxSize**(*minWidth, minHeight, maxWidth, maxHeight*);
  - �E�B�W�F�b�g�̍ŏ��ő�T�C�Y���܂Ƃ߂Đݒ肷��
- **setFixedSize**(*width, height*);
  - �E�B�W�F�b�g���Œ�T�C�Y�ɐݒ肷��
- **setFixedSizeToImageSize**(*width, height*);
  - ***Layer#loadImages***()�Ń��[�h�����摜�T�C�Y�ɕ����ăE�B�W�F�b�g���Œ�T�C�Y�ɐݒ肷��
- **drawUIText**(*fontStyle, x, y, text, overrideFontColor = void*);
  - �X�^�C���Ŏw�肳�ꂽ�t�H���g�X�^�C���ɉ�����UI�e�L�X�g��`�悷��B

	*x*,*y*�Ŏw�肵�����W��*text*�ɕ`�悷��B

	*overrideFontColor*���w�肷��ƁA*fontStyle*���̂̏㏑���Ȃ���
	�t�H���g�J���[�̂ݕύX���ĕ`����s����B

- object **drawUITextInRange**(*fontStyle, x, y, w, text, alignment, fill = false*);
  - �w��͈͂�UI�e�L�X�g��`�悷��B

	**drawUIText**() ���l�Ƀe�L�X�g��`�悷�邪�A
	*w*�Ŏw�肵�������I�[�o�[�������͏ȗ��\�������B
	�e�L�X�g�����w�蕝�ɑ���Ȃ��ꍇ�� *alignment* �ɂ���������
	�E�l�߁E�Z���^�����O�E���l�߂̂����ꂩ�ŕ\�������B
	*fill* ���w�肷��ƃe�L�X�g�`��O�� �o�b�N�O���E���h�J���[�Ŕw�i���t�B������B

  - **�Ԃ�l**
	- %[  
		**inRange** (bool) �e�L�X�g���w��͈͓��ɕ`�悳�ꂽ���ǂ���  
		**x** (int) ���ۂɃe�L�X�g���`�悳�ꂽX���W  
		**y** (int) ���ۂɃe�L�X�g���`�悳�ꂽY���W  
		**w** (int) ���ۂɃe�L�X�g���`�悳�ꂽ��  
		]

- **extractTextArea**(*fontStyle, text, lineSpacing*);
  - �e�L�X�g���e�L�X�g�G���A�ɕ������A�`�揀�����s���B

	�e�L�X�g���A�s���Ƃɕ������A
	�w��̃X�^�C��(*fontStyle*)�ƍs��(*lineSpacing*)�ɏ]���ăT�C�Y�v�Z���s��
	�e�L�X�g�G���A�̕`�揀�����s���B

	���̊֐��̕Ԃ����l�� **drawTextArea**() �֐��ɓn���ăe�L�X�g�G���A��`�悷��B

  - **�Ԃ�l**
	- %[  
		**texts** (Array) �s���Ƃɕ������ꂽ�e�L�X�g�̔z��  
		**textWidths** (Array) �s���Ƃ̃e�L�X�g�̕`�敝�̔z��  
		**width** (int) �e�L�X�g�G���A�S�̂̕�  
		**height** (int) �e�L�X�g�G���A�S�̂̍���  
		]

- **drawTextArea**(*fontStyle, x, y, w, lineSpacing, texts, textWidths, textAlign*);
  - �e�L�X�g�G���A��`�悷��

	  **extractTextArea**() �œ���ꂽ�e�L�X�g�G���A���Ɋ�Â���
	  �e�L�X�g�G���A��`�悷��B

	  �e�s�̒������e�L�X�g�G���A�̑S�̕��ɖ����Ȃ��ꍇ�́AtextAlign�Ɋ�Â���
	  ���񂹁E�Z���^�����O�E�E�񂹂Ŋe�s���A���C�����g�����B

- **drawBorder**(*borderStyle, x, y, w, h, clear = true*);
  - �{�[�_�[��`�悷��

	  *borderStyle*�ɉ����āA*x,y,w,h*�Ŏw�肵���̈�Ƀ{�[�_�[��`�悷��B
	  *clear*���w�肷��ƕ`��O��*backgroundColor*�ŗ̈���N���A����B

- **drawChecker**(*l, t, w, h, csize, color1 = 0xFFFFFFFF, color2 = 0xFFC0C0C0, sx = 0, sy = 0*);
  - �`�F�b�J�[(�s���͗l)��`�悷��B

	*(l,t,w,h)*�Ŏw�肵���̈��*color1*��*color2*�̓�F��*csize*�T�C�Y�̋�`��
	�\�����ꂽ�s���͗l��`�悷��B
	*sx,sy*���w�肷��Ǝs���͗l�̕`��J�n�ʒu���V�t�g����B

- **fillChecker**(*csize, color1 = 0xFFFFFFFF, color2 = 0xFFC0C0C0*);
  - �E�B�W�F�b�g�̑S����`�F�b�J�[�Ńt�B������B

- object **getChildsBounds**(*child*);
	- �}�[�W���A�p�f�B���O�A�{�[�_�[�����ׂĊ܂񂾁u�q�E�B�W�F�b�g�̃o�E���Y�v��Ԃ��B
	- **�Ԃ�l**  
		%[  
		**minWidth** (int)  �q�E�B�W�F�b�g��z�u����̂ɕK�v�ȋ󔒂��܂� minWidth
		**maxWidth** (int) �q�E�B�W�F�b�g��z�u����̂ɕK�v�ȋ󔒂��܂� maxWidth
		**minHeight** (int) �q�E�B�W�F�b�g��z�u����̂ɕK�v�ȋ󔒂��܂� minHeight
		**maxHeight** (int) �q�E�B�W�F�b�g��z�u����̂ɕK�v�ȋ󔒂��܂� maxHeight

- **embedChild**(*x, y, w, h, child, clear = false*);
  - �w�肵���̈�Ɏq�E�B�W�F�b�g�𖄂ߍ��ށB

	  �w�肵���̈�Ƀ{�[�_�[��`�悵����ŁA�K�؂ȋ󔒂������Ďq�E�B�W�F�b�g��z�u����B
	  getChildBounds() �ƕ����Ĉȉ��̂悤�Ɏg���B

		var cb = getChildBounds(child);
		setMinMaxSize(cb.minWidth, cb.minHeight, cb.maxWidth, cb.maxHeight);
		embedChild(child, 0, 0, width, height);

## �t�b�N
- **onExtractWidgetStyle**(*widgetStyle*);
  - �E�B�W�F�b�g�ŗL�̃X�^�C����W�J����^�C�~���O�ŌĂяo�����B  
	KWidget�̕W���X�^�C���v���p�e�B�ȊO��
	�E�B�W�F�b�g�ŗL�̃X�^�C���v���p�e�B���`�������ꍇ�́A
	���̃t�b�N���Ăяo���ꂽ�^�C�~���O�ňȉ��̂悤�Ƀv���p�e�B���擾�ł���B

		function onExtractWidgetStyle(widgetStyle) {
		  widgetStyle.lineSpacing = getStyleProperty("lineSpacing", 0);
		}

- **onAttachedToWindow**();
  - �E�B�W�F�b�g���E�B���h�E�ɃA�^�b�`���ꂽ�^�C�~���O�ŌĂяo�����
- **onDetachedFromWindow**();
  - �E�B�W�F�b�g���E�B���h�E����Ń^�b�`���ꂽ�^�C�~���O�ŌĂяo�����
- **onBindPersistentDictionary**(*dictionary*);
  - �i���������ƃo�C���h�����^�C�~���O�ŌĂяo�����  
  ��������v���p�e�B�̏����l��ǂ݂����̂ɗ��p����B
- **onUnbindPersistentDictionary**(*dictionary*);
  - �i���������Ƃ̃o�C���h�����������^�C�~���O�ŌĂяo�����B  
  �����Ƀv���p�e�B�̌��ݒl���L�^����̂ɗ��p����B
- **onValueModified**(*newValue* );
  - **�l**�����E�B�W�F�b�g�Œl���ύX���ꂽ�^�C�~���O�ŌĂяo�����B

	  **�f�t�H���g����**  
	  �e�� onChildValueModified() ���Ăяo���B

- **onChildValueModified**(*child, newValue* );
  - �q�̒l���ύX���ꂽ�^�C�~���O�ŌĂяo�����B

	  **�f�t�H���g����**  
	  �e�� onChildValueModified()�ɂ��̂܂܃f���Q�[�g����B
	  ���̓���ɂ��A�E�B�W�F�b�g�c���[�̏�ʊK�w�ŁA
	  �q���E�B�W�F�b�g�̒l�̕ύX���܂Ƃ߂ĕ⑫���邱�Ƃ��o����B

