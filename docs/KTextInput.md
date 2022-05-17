# KTextInput

<details>
<summary>�}��</summary>

	new KTextInput(window,
		%[
			name: "textinput",
			type: TEXT_REAL,
			width: 100,
			height: 20,
			value: 3.141592
		]
		);

</details>

![KTextInput](KTextInput.png)

KTextInput �́A�w�肵���t�H�[�}�b�g�̂ݎ󂯕t����e�L�X�g�t�B�[���h�ł��B

## �e�N���X

**KTextInput** -> [KValueEntity](KValueEntity.md) -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## �����N���X

- [KTextEdit](KTextEdit.md)

## �R���X�g���N�^
```KTextInput(window, options = %[])```

### �I�v�V��������(�^: �����l)
- **focusable** (bool: true)
  - �t�H�[�J�X�\
    true���w�肷��ƃt�H�[�J�X�\�ɂȂ�܂��B
- **width** (int)
  - �e�L�X�g�t�B�[���h�̕�
- **height** (int)
  - �e�L�X�g�t�B�[���h�̍����B  
    �ȗ�����ƃt�H���g�T�C�Y���玩���v�Z����܂��B
- **type** (int: TEXT_SYMBOL)
  - �e�L�X�g�^�C�v  
	���͂��󂯕t����e�L�X�g�̃^�C�v�� [TextType](KDefs.md#texttype) �Ŏw�肵�܂��B
- **hideText** (bool: false)
  - �e�L�X�g���B�����ǂ������w�肵�܂��B  
  true�ɐݒ肷��Ɠ��͂����e�L�X�g�́u*�v�̕\���ŃJ�o�[����܂��B
  �p�X���[�h�̓��͂ȂǂɎg���܂��B
- **respondToValueModification** (bool: false)
  - �e�L�X�g�̍X�V�ɔ������Ė��� dispatch() �𔭍s���邩�ǂ�����ݒ肵�܂��B
- **value** (any)
  - �l�̏����l

## �X�^�C��
- **borderStyle**, **fontStyle**
  - �X�^�C���w��Ɋ�Â��A�w�i���t�B������܂�
- **texteditId** (string: "textinput_textedit")
  - KTextInput���̃e�L�X�g�t�B�[���h�ɖ��ߍ��܂ꂽKTextEdit ��id�ł�  
	�e�L�X�g�t�B�[���h�̃X�^�C���͎w�肵��id�ɑ΂��ăJ�X�^�}�C�Y�ł��܂��B

## �v���p�e�B
- **hideText** (bool)
  - �e�L�X�g���B�����ǂ����B
- **respondToValueModification** (bool)
  - �e�L�X�g�̍X�V�ɔ������Ė��� dispatch() �𔭍s���邩�ǂ����B
- **value** (string)
  - �e�L�X�g�̒l

## �t�b�N
- **onEnter**();
  - Enter�L�[�������ꂽ�^�C�~���O�ŌĂяo����܂��B
- **onCancel**();
  - Esc�L�[�������ꂽ�^�C�~���O�ŌĂяo����܂��B  
