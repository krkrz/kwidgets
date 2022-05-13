# KVirtualWidget

KVirtualWidget は
スクロール領域の内、実際に画面に表示される狭い領域のみにレイヤの実体を割り当てることで、
少ないメモリ消費で広大な領域の描画を可能にする仕組みです。

KScrollLayout に格納した KVirtualWidget の表示領域が更新されると、
新たに表示された領域の描画を行うため onDraw() フックが呼び出されます。

onDraw() フック内では、通常の吉里吉里の
[Layer](https://krkrz.github.io/docs/kirikiriz/j/contents/f_Layer.html)
と同様の描画関数群を使ってレイヤへの描画をリクエストすることができます。

実際にはこれらの描画リクエストは、表示領域と同等のサイズしか持たない
内部的な **canvas** レイヤに転送して処理されます。

## 親クラス

**KVirtualWidget** -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## コンストラクタ
```KVirtualWidget(window, options = %[])```

## プロパティ
- Layer **canvas** (readonly)
  - 実際の描画が行われるキャンバスレイヤ
- **siteLeft** (int)
  - 現在のcanvasの、スクロール領域中に占める左端座標
- **siteTop** (int)
  - 現在のcanvasの、スクロール領域中に占める上端座標
- **siteWidth** (int)
  - 現在のcanvasの、スクロール領域中に占める幅
- **siteWidth** (int)
  - 現在のcanvasの、スクロール領域中に占める高さ
- **avoidSiteUpdateDrawing** (bool)
  - 領域更新時の再描画を行うかどうか

	このプロパティにtrueを指定すると、一時的にサイト更新時の再描画が抑制されます。
	描画リクエストはキャッシュされ、falseを指定した瞬間に再描画が実行されます。

	ウィジェットに再描画を伴う大きな変更を連続して加える際、
	更新が終わるまで描画を抑制するといった目的で使用します。

## メソッド
- **redrawRect**(*left, top, width, height*);
  - ウィジェット中の指定した領域に再描画リクエストを送ります。
- **redrawAll**();
  - ウィジェットの全域に再描画リクエストを送ります。

## 静的メソッド
- **registerDelegateClasss**(*delegateClass*);
  - デリゲートクラスを登録します。
- **unregisterDelegateClasss**(*delegateClass*);
  - デリゲートクラスを解除します。

## フック
- **onDraw(left, top, width, height);
  - canvasへの再描画が必要なタイミングで呼ばれます。

	領域のスクロール、ウィジェットのリサイズ、描画リクエストの発行など、
	様々な要因で再描画が必要になったタイミングで呼び出されるフックです。

	通常のLayerの更新と同様の手順で、指定の領域へ描画処理を実行してください。

## 実装済みのデリゲート描画関数
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

## 任意の描画関数をデリゲート対象にする
KVirtualWidgetにビルトインされていない描画関数を後付けでデリゲート対象にすることができます。

例として、DLL経由でLayerクラスに以下のような仕様の描画関数が追加された場合を考えます。

	function grayscaleRect(left, top, width, height, rate);
	// 指定の領域を変化率を指定してグレースケール化する関数

準備として、まず以下のようなデリゲートクラスを作ります。

	class MyVirtualWidgetDelegate
		function grasyaleRect(left, top, width, height, rate) {
			canvas.grayscaleRect(left - siteLeft, top - siteTop, width, height, rate);
		}
	}

**KVirtualWidget#grayscaleRect**() が呼び出されたときに、
座標をあらわす *left*と*top*の引数を、
視界のスクロール量である *siteLeft*と*siteTop*を使って補正した上で、
実際の描画先であるcanvasに対してデリゲートするようにしています。

あとは、以下のようにしてこのデリゲートクラスをKVirtualWidgetに登録すれば対応完了です。

	KVirtualWidget.registerDelegateClass(MyVirtualWidgetDelegate);


