# KRadioLayout

KRadioLayout は、子として登録されている複数のウィジェットの内、
1つのウィジェットのみを「選択状態」にするラジオボックスです。

通常、子としては [KRadioButton](KRadioButton.md) を使いますが、
trueとfalseの二値を取るウィジェットならば対象は何でもかまいません。

配置は以下のように行います。

	var radioBox = new KRadioLayout(window, %[ name: "radioBox" ]);
	radioBox.add(0, 0, new KRadioButton(window, %[ label: "選択肢1", choice: 1 ]));
	radioBox.add(0, 1, new KRadioButton(window, %[ label: "選択肢2", choice: 2 ]));
	radioBox.add(0, 2, new KRadioButton(window, %[ label: "選択肢3", choice: 3 ]));
	radioBox.value = 2;

KRadioLayout は [KGridLayout](KGridLayout.md) の継承クラスなので、
KGridLayout同様にグリッド上に配置を行えます。

子である KRadioButton のオプション引数 **choice** が、
そのウィジェットを選択した際の「ラジオボックスの値」となります。

## 親クラス
**KRadioLayout** -> [KGridLayout](KGridLayout.md) -> [KLayout](KLayout.md) -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## コンストラクタ
```KRadioLayout(window, options = %[])```

### オプション引数
- **value** (var: void)
  - 初期値
