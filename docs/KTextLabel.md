# KTextLabel

KTextLabel は、任意のテキストを表示するラベルです。

## 親クラス

**KTextLabel** -> [KLabel](KLabel.md) -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## コンストラクタ
```KTextLabel(window, options = %[])```

### オプション引数(型: 初期値)
- **label** (string)
  - テキストラベル  
  ボタンに表示されるテキストラベルを指定します。

## プロパティ
- **label** (string)
  - テキストラベル

## スタイル(型: 初期値)
- **lineSpacing** (int: 0)
  - 行間  
	テキストラベルが改行文字を含む場合の行間サイズを指定します。
- **textAlign** ([Alignment](KDefs.md#alignment) : ALIGN_LEFT)
  - テキストアライメント  
	variableWidth = true 時にテキストを配置するアライメントを指定します。
- **maxLabelWidth** (int: void)
  - ラベルの最大表示幅  
  ラベルの最大表示幅を明示的に指定します。
