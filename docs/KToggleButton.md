# KToggleButton

KToggleButton は、押すたびにオン/オフ状態を切り替える
トグルボタンのウィジェットです。

## 親クラス

**KToggleButton** -> [KValueEntity](KValueEntity.md) -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## コンストラクタ
```KToggleButton(window, options = %[])```

### オプション引数(型: 初期値)
- **value** (bool: false)
  - 値の初期値です。
- **focusable** (bool: false)
  - フォーカス可能  
    trueを指定するとフォーカス可能になります。
- **isVariableWidth** (bool: false)
  - 可変幅指定。  
	true を指定すると、ボタンの横幅が可変長になります。

## スタイル
- **borderStyle**, **fontStyle**
  - スタイル指定に基づき、ボーダーが表示されます。

## カスタムスタイルステート
- **toggleOn**
  - トグルがオンの時に有効になるステートです。
- **toggleOff**
  - トグルがオフの時に有効になるステートです。

## プロパティ
- **value** (bool)
  - トグルの値です。


