# KEntity

KEntity は実体を持つウィジェットのベースとなるクラスです。

下位互換性を維持するための歴史的経緯で KWidget と分離した形で定義されていますが、
kwidgets ver2 以降は、この KEntity を実質上
すべてのウィジェットの基底クラスとして扱ってかまいません。

## 親クラス

**KEntity** -> [KWidget](KWidget.md)

## コンストラクタ
```KEntity(window, options = %[])```

## メソッド
- **redarwContents**();
  - コンテンツの再描画をリクエストします。

	条件を満たしていれば即座に **onDrawContents**() が呼び出されます。

	ウィンドウにアタッチされていない時など、
	条件によっては即座に呼び出しが行われず、
	遅延して実行が行われます。

## フック
- **onDrawContents**();
  - コンテンツの再描画が必要なタイミングで呼び出されます。

	継承クラスで適切に実装して、コンテンツを描画するコマンドを記述します。
	
	*redrawContents*() で明示的に呼び出す以外に、
	エンティティのスタイルやサイズが変更され、
	システム的に再描画が必要になったタイミングでも呼び出されます。