# KButton

KButton は、ボタンを押したイベントを通知するウィジェットです。

## 親クラス

**KButton** -> [KValueEntity](KValueEntity.md) -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## コンストラクタ
```KButton(window, options = %[])```

### オプション引数(型: 初期値)
- **value** (var: void)
  - ボタンの値
	ディスパッチ時に onValueModified() に送られる「値」を指定できます。
- **focusable** (bool: false)
  - フォーカス可能  
    trueを指定するとフォーカス可能になります。
- **isVariableWidth** (bool: false)
  - 可変幅指定。  
	true を指定すると、ボタンの横幅が可変長になります。
- **dispatchOnMouseDown** (bool: false)
  - ディスパッチタイミングの切り替え  
	デフォルト動作ではボタンの解放時に dispatch() が実行されますが、
	このオプションをtrueに指定すると
	ボタンの押下時に dispatch() が実行されるようになります。
- **autoDispatchStartInterval** (int: void)
  - 自動ディスパッチ開始までのインターバル(ms)  
- **autoDispatchRepeatInterval** (int: void)
  - 自動ディスパッチを繰り返すインターバル(ms)  
	これらのオプションを指定すると、ボタンを押しっぱなしにすることで、
	連続でボタンを押し続けるオートディスパッチ動作がオンになります。

## スタイル
- **borderStyle**, **fontStyle**
  - スタイル指定に基づき、ボーダーが表示されます。


