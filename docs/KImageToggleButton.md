# KImageToggleButton

<details>
<summary>凡例</summary>

	new KImageToggleButton(window,
		%[
			name: "imagebutton",
			icon: "gotta_mail",
			value: true,
		]
		);

</details>

![KImageToggleButton](KImageToggleButton.png)

KImageToggleButton は、任意のアイコンイメージを表示するトグルボタンです。

このクラスでは、表示するアイコンイメージを
コンストラクト時のオプション引数だけでなく、
スタイルでも指定することができます。

これにより、スタイルステートが変わった時に
アイコンも変えることができます。

指定が重複した場合はスタイル指定が優先されます。

## 親クラス

**KImageToggleButton** -> [KButton](KToggleButton.md) -> [KValueEntity](KValueEntity.md) -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## コンストラクタ
```KImageButton(window, options = %[])```

### オプション引数(型: 初期値)
- **icon** (string)
  - アイコン  
  ボタンに表示されるアイコンを指定します。

## プロパティ
- **icon** (string)
  - アイコン

## スタイル(型: 初期値)
- **icon** (string: "")
  - アイコン  
	ボタンに表示するアイコンを指定します。
- **opacity** (int: 255)
  - 不透明度  
	アイコンの不透明度を指定します。

