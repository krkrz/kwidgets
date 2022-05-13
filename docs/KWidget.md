# KWidget

KWidgetはすべてのウィジェットの既定となるベースクラス。

吉里吉里のLayer クラスの継承クラスなので、
Layerが対応している描画関数はすべて呼び出せる。

## 親クラス

**KWidget** -> [Layer](https://krkrz.github.io/docs/kirikiriz/j/contents/f_Layer.html)

## コンストラクタ
```KWidget(window, options = %[])```
### オプション引数(型: 初期値)
- **name** (string: "")
  - 名前

	UIツリー上のウィジェットは指定した名前で識別可能になります。

- **id** (string: "")
  - ID

	 IDを設定することで対応した特定のスタイルを指定可能になります。

- **style** (dictionary: %[])
  - スタイル

	 ウィジェットの見た目を定義するスタイルを指定します。

- **status** (string: "")
  - ステータス

	ステータスを指定すると、このウィジェットのマウスホバー時に
	[KStatusBar](KStatusBar.md) に対応したステータステキストが
	表示されるようになります。

- **hint** (string: "")
  - ヒントテキスト

	ヒントを指定すると、このウィジェットのマウスホバー時に
	ポップアップウィンドウでヒントテキストが表示されるようになります。

- **isAvoidDictionaryRegistration** (bool: false)
  - 永続化辞書への登録を回避する

## プロパティ
- **id** (string)
  - ID
- **style** (dictionary)
  - スタイル
- **status** (string)
  - ステータス
- **hint** (string)
  - ヒントテキスト
- **isAvoidDictionaryRegistration** (bool)
  - 永続化辞書への登録を回避する
- **ancestors** (Array)
  - 自分自身の祖先ウィジェットの一覧
- **descendants** (Array)
  - 自分自身の子孫ウィジェットの一覧
- **minWidth** (int)
  - ウィジェットの最小幅
- **maxWidth** (int)
  - ウィジェットの最大幅
- **minHeight** (int)
  - ウィジェットの最小高さ
- **maxHeight** (int)
  - ウィジェットの最大高さ
- **widgetStyle** (readonly dictionary)
  - ウィジェット固有のスタイル  
	KWidgetの標準スタイルとは別に、継承クラスでウィジェット固有のスタイル定義を格納するための辞書。
- **fontStyle** (readonly dictionary)
  - スタイルチェーンから抽出されたウィジェットの現在のフォントスタイル  
  - %[  
	**face** (string) フェイス  
	**height** (int) ハイト  
	**color** (int) カラー(0xRRGGBB)  
	**antiAlias** (bool) アンチエリアスの有無  
	**outlline** (bool) アウトラインの有無  
	**outlineColor** (int) アウトラインカラー(0xRRGGBB)  
	**bold** (bool) ボールドの有無  
	**italic** (bool) イタリックの有無  
	**strikeout** (bool) ストライクアウトの有無  
	**underline** (bool) アンダーラインの有無  
	]

- **borderStyle** (readonly dictionary)
  - スタイルチェーンから抽出されたウィジェットの現在のボーダースタイル
  - %[  
	**backgroundColor** (int) 背景カラー(0xAARRGGBB)  
	**width{Left|Right|Top|Bottom}** (int) 左/右/上/下幅  
	**color{Left|Right|Top|Bottom}** (int) 左/右/上/下カラー(0xRRGGBB)  
	**style{Left|Right|Top|Bottom}** (int) 左/右/上/下スタイル  
	**radius{LeftTop|RightTop|LeftBottom|RightBottom}** (int) 左上/右上/左下/右下半径  
	]

- **locateStyle** (readonly dictionary)
  - スタイルチェーンから抽出されたウィジェットの現在のロケートスタイル
  - %[  
	**margin{Left|Right|Top|Bottom}** (int) ウィジェットの左/右/上/下に挿入される空きスペースのサイズ  
	**alignHorizontal** (int) 横に空きスペースがある際にウィジェットをアライメントする方向  
	**alignVertical** (int) 縦に空きスペースがある際にウィジェットをアライメントする方向  
	]
- **marginLeft** (int)
  - locateStyle.marginLeft のショートカット
- **marginRight** (int)
  - locateStyle.marginRight のショートカット
- **marginTop** (int)
  - locateStyle.marginTop のショートカット
- **marginBottom** (int)
  - locateStyle.marginBottom のショートカット
- **alignHorizontal** (int)
  - locateStyle.alignHorizontal のショートカット
- **layoutStyle** (readonly dictionary)
  - スタイルチェーンから抽出されたウィジェットの現在のレイアウトスタイル
  - %[  
	**spaceHorizontal** (int) 子ウィジェットを横に並べて配置する際挿入される空きスペースのサイズ  
	**spaceVertidcal** (int) 子ウィジェットを縦に並べて配置する際挿入される空きスペースのサイズ  
	**padding{Left|Right|Top|Bottom>** (int) 子ウィジェットの配置時に左/右/上/下側に開けるスペースのサイズ  
	]
- **spaceHorizontal** (int)
  - layoutStyle.spaceHorizontal のショートカット
- **spaceVertical** (int)
  - layoutStyle.spaceVertical のショートカット
- **paddingLeft** (int)
  - layoutStyle.paddingLeft のショートカット
- **paddingRight** (int)
  - layoutStyle.paddingRight のショートカット
- **paddingTop** (int)
  - layoutStyle.paddingTop のショートカット
- **paddingBottom** (int)
  - layoutStyle.paddingBottom のショートカット

- **borderPaddingLeft** (int)
  - layoutStyle.paddingLeft + borderStyle.widthLeft  
  ボーダーとパディングの左側サイズの合計。  
  子ウィジェットを配置する際の左側の余白の基準
- **borderPaddingRight** (int)
  - layoutStyle.paddingRight + borderStyle.widthRight  
  ボーダーとパディングの右側サイズの合計。  
  子ウィジェットを配置する際の右側の余白の基準
- **borderPaddingTop** (int)
  - layoutStyle.paddingTop + borderStyle.widthTop  
  ボーダーとパディングの上側サイズの合計。  
  子ウィジェットを配置する際の上側の余白の基準
- **borderPaddingBottom** (int)
  - layoutStyle.paddingBottom + borderStyle.widthBottom  
  ボーダーとパディングの下側サイズの合計。  
  子ウィジェットを配置する際の下側の余白の基準
- **borderPaddingWidth** (int)
  - borderPaddingLeft + borderPaddingRight;
- **borderPaddingHeight** (int)
  - borderPaddingTop + borderPaddingBottom;



## メソッド
- var **getOption**(*key, defaultValue = void*);
  - オプション引数を取得する。  
	コンストラクタで指定されたオプション引数から、
	特定の*key*に対応した値を取得する。
	対応する*key*が存在しない時には、
	*defaultValue*が返ってくる。

- var **getStyleProperty**(*key, defaultValue = void*);
  - スタイルのプロパティを取得する。  
	現在のスタイルチェーンから、
	*key*に対応したプロパティを検索してその値を取得する。
	*key*に対応するプロパティが定義されていない場合には
	*defaultValue*が返ってくる。

- object **find**(*name*);
  - ウィジェットツリーから特定の名前を持つウィジェットを検索します。
- bool **isAncestorOf**(*widget*);
  - 指定の*widget*が自分の先祖であるか判定する。
- bool **isDescendantOf**(*widget*);
  - 指定の*widget*が自分の先祖であるか判定する。
- **setMinSize**(*minWidth, minHeight*);
  - ウィジェットの最小サイズを設定する
- **setMaxSize**(*maxWidth, maxHeight*);
  - ウィジェットの最大サイズを設定する
- **setMinMaxSize**(*minWidth, minHeight, maxWidth, maxHeight*);
  - ウィジェットの最小最大サイズをまとめて設定する
- **setFixedSize**(*width, height*);
  - ウィジェットを固定サイズに設定する
- **setFixedSizeToImageSize**(*width, height*);
  - ***Layer#loadImages***()でロードした画像サイズに併せてウィジェットを固定サイズに設定する
- **drawUIText**(*fontStyle, x, y, text, overrideFontColor = void*);
  - スタイルで指定されたフォントスタイルに応じてUIテキストを描画する。

	*x*,*y*で指定した座標じ*text*に描画する。

	*overrideFontColor*を指定すると、*fontStyle*自体の上書きなしに
	フォントカラーのみ変更して描画を行える。

- object **drawUITextInRange**(*fontStyle, x, y, w, text, alignment, fill = false*);
  - 指定範囲にUIテキストを描画する。

	**drawUIText**() 同様にテキストを描画するが、
	*w*で指定した幅をオーバーした分は省略表示される。
	テキスト幅が指定幅に足りない場合は *alignment* にしたがって
	右詰め・センタリング・左詰めのいずれかで表示される。
	*fill* を指定するとテキスト描画前に バックグラウンドカラーで背景をフィルする。

  - **返り値**
	- %[  
		**inRange** (bool) テキストが指定範囲内に描画されたかどうか  
		**x** (int) 実際にテキストが描画されたX座標  
		**y** (int) 実際にテキストが描画されたY座標  
		**w** (int) 実際にテキストが描画された幅  
		]

- **extractTextArea**(*fontStyle, text, lineSpacing*);
  - テキストをテキストエリアに分割し、描画準備を行う。

	テキストを、行ごとに分割し、
	指定のスタイル(*fontStyle*)と行間(*lineSpacing*)に従ってサイズ計算を行い
	テキストエリアの描画準備を行う。

	この関数の返した値を **drawTextArea**() 関数に渡してテキストエリアを描画する。

  - **返り値**
	- %[  
		**texts** (Array) 行ごとに分割されたテキストの配列  
		**textWidths** (Array) 行ごとのテキストの描画幅の配列  
		**width** (int) テキストエリア全体の幅  
		**height** (int) テキストエリア全体の高さ  
		]

- **drawTextArea**(*fontStyle, x, y, w, lineSpacing, texts, textWidths, textAlign*);
  - テキストエリアを描画する

	  **extractTextArea**() で得られたテキストエリア情報に基づいて
	  テキストエリアを描画する。

	  各行の長さがテキストエリアの全体幅に満たない場合は、textAlignに基づいて
	  左寄せ・センタリング・右寄せで各行がアライメントされる。

- **drawBorder**(*borderStyle, x, y, w, h, clear = true*);
  - ボーダーを描画する

	  *borderStyle*に応じて、*x,y,w,h*で指定した領域にボーダーを描画する。
	  *clear*を指定すると描画前に*backgroundColor*で領域をクリアする。

- **drawChecker**(*l, t, w, h, csize, color1 = 0xFFFFFFFF, color2 = 0xFFC0C0C0, sx = 0, sy = 0*);
  - チェッカー(市松模様)を描画する。

	*(l,t,w,h)*で指定した領域に*color1*と*color2*の二色で*csize*サイズの矩形で
	構成された市松模様を描画する。
	*sx,sy*を指定すると市松模様の描画開始位置をシフトする。

- **fillChecker**(*csize, color1 = 0xFFFFFFFF, color2 = 0xFFC0C0C0*);
  - ウィジェットの全域をチェッカーでフィルする。

- object **getChildsBounds**(*child*);
	- マージン、パディング、ボーダーをすべて含んだ「子ウィジェットのバウンズ」を返す。
	- **返り値**  
		%[  
		**minWidth** (int)  子ウィジェットを配置するのに必要な空白を含んだ minWidth
		**maxWidth** (int) 子ウィジェットを配置するのに必要な空白を含んだ maxWidth
		**minHeight** (int) 子ウィジェットを配置するのに必要な空白を含んだ minHeight
		**maxHeight** (int) 子ウィジェットを配置するのに必要な空白を含んだ maxHeight

- **embedChild**(*x, y, w, h, child, clear = false*);
  - 指定した領域に子ウィジェットを埋め込む。

	  指定した領域にボーダーを描画した上で、適切な空白を加えて子ウィジェットを配置する。
	  getChildBounds() と併せて以下のように使う。

		var cb = getChildBounds(child);
		setMinMaxSize(cb.minWidth, cb.minHeight, cb.maxWidth, cb.maxHeight);
		embedChild(child, 0, 0, width, height);

## フック
- **onExtractWidgetStyle**(*widgetStyle*);
  - ウィジェット固有のスタイルを展開するタイミングで呼び出される。  
	KWidgetの標準スタイルプロパティ以外に
	ウィジェット固有のスタイルプロパティを定義したい場合は、
	このフックが呼び出されたタイミングで以下のようにプロパティを取得できる。

		function onExtractWidgetStyle(widgetStyle) {
		  widgetStyle.lineSpacing = getStyleProperty("lineSpacing", 0);
		}

- **onAttachedToWindow**();
  - ウィジェットがウィンドウにアタッチされたタイミングで呼び出される
- **onDetachedFromWindow**();
  - ウィジェットがウィンドウからでタッチされたタイミングで呼び出される
- **onBindPersistentDictionary**(*dictionary*);
  - 永続化辞書とバインドされるタイミングで呼び出される  
  辞書からプロパティの初期値を読みだすのに利用する。
- **onUnbindPersistentDictionary**(*dictionary*);
  - 永続化辞書とのバインドが解除されるタイミングで呼び出される。  
  辞書にプロパティの現在値を記録するのに利用する。
- **onValueModified**(*newValue* );
  - **値**を持つウィジェットで値が変更されたタイミングで呼び出される。

	  **デフォルト動作**  
	  親の onChildValueModified() を呼び出す。

- **onChildValueModified**(*child, newValue* );
  - 子の値が変更されたタイミングで呼び出される。

	  **デフォルト動作**  
	  親の onChildValueModified()にそのままデリゲートする。
	  この動作により、ウィジェットツリーの上位階層で、
	  子孫ウィジェットの値の変更をまとめて補足することが出来る。

