# KCheckBox

KCheckBox は、オンオフのチェックを切り替えられるラベルつきのチェックボックスです。

## 親クラス

**KCheckBox** -> [KValueEntity](KValueEntity.md) -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)
## コンストラクタ
```KCheckBox(window, options = %[])```

### オプション引数(型: 初期値)
- **value** (bool: false)
  - 初期値
- **focusable** (bool: false)
  - フォーカス可能  
    trueを指定するとフォーカス可能になります。
- **isVariableWidth** (bool: false)
  - 可変幅指定。  
	true を指定すると、チェックボックスの横幅が可変長になります。
- **label** (string: "")
  - チェックの横に表示されるラベルの文字列です。

## スタイル
- **borderStyle**, **fontStyle**
  - スタイル指定に基づき、ボーダーが表示されます。

## スタイル(型: 初期値)
- **icon** (string: "")
  - アイコン  
	ボタンに表示するアイコンを指定します。
- **opacity** (int: 255)
  - 不透明度  
	アイコンの不透明度を指定します。

## カスタムスタイルステート
- **checkOn**
  - チェックがオンの時に有効になるステートです。
- **checkOff**
  - チェックがオフの時に有効になるステートです。

## プロパティ
- **value** (bool)
  - チェックボックスの値です。
- **label** (string: "")
  - チェックの横に表示されるラベルの文字列です。
