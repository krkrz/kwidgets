# KTextEdit

KTextEditは、長文のテキストを編集できるテキストエディタクラスです。

目的に応じてキーバインドを自由にカスタマイズ可能です。

デフォルトで、Windowsキーバインドと、emacsキーバインドが使えるようになっています。

## 親クラス

**KTextEdit** -> [KVirtualWidget](KVirtualWidget.md) -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## コンストラクタ
```KTextEdit(window, options = %[])```

### オプション引数(型: 初期値)
- **tabWidth** (int: 4) 
  - KTextEditに値を設定した際、tabはスペースに展開されます。
  その文字数を設定します。
- **lineWrapping** (bool: true)
  - 右端で行を折り返すかどうかを設定します。
- **hideText** (bool: false)
  - テキストを隠すかどうかを指定します。  
  trueに設定すると入力したテキストは「*」の表示でカバーされます。
  パスワードの入力などに使います。
- **respondToValueModification** (bool: false)
  - テキストの更新に反応して毎回 dispatch() を発行するかどうかを設定します。
- **value** (string: "")
  - ウィジェットの初期値  
	コンストラクト時点での値を設定します。

## スタイル
- **borderStyle**, **fontStyle**
  - スタイル指定に基づき、背景がフィルされ、フォントが描画されます。
- **selectionBackgroundColor**
  - 選択範囲の背景色を指定します。(0xAARRGGBB)
- **selectionFontColor**
  - 選択範囲のフォントカラーを指定します。(0xRRGGBB)

## プロパティ
- **tabWidth** (int)
  - tabの文字数
- **lineWrapping** (bool)
  - 右端で行を折り返すかどうか。
- **hideText** (bool)
  - テキストを隠すかどうか。
- **respondToValueModification** (bool)
  - テキストの更新に反応して毎回 dispatch() を発行するかどうか。
- **value** (string)
  - テキストの値

## メソッド
- **clearKeyBind**();
  - キーバインドをすべてクリアします。
- **bindKey**(*key, shiftSelect, modifier, func*);
  - キーバインドを登録します。
  キーコード*key*とモディファイヤ*modifier*で指定されるキーと特定の関数*func*を関連付けます。  
	*shiftSelect*に1を指定すると、shiftキーを押しながらの選択範囲の移動ができるようになります。  
	*shiftSelect*に2を指定すると、操作に伴い選択範囲解除しないようになります。
- **bindWindoskeyMap**();
  - Windowsのメモ帳仕様のキーマップをバインドします。
- **bindEmacsKeyMap**();
  - Emacs仕様のキーマップをバインドします。

## テキストエディタ機能メソッド
- **moveLeft**();
  - ポインタを1文字左に動かします。
- **moveRight**();
  - ポインタを1文字右に動かします。
- **moveUp**();
  - ポインタを1文字上に動かします。
- **moveDown**();
  - ポインタを1文字下に動かします。
- **backwardWord**();
  - ポインタを1単語分後ろに動かします。
- **forwardWord**();
  - ポインタを1単語分前に動かします。
- **backwardParagraph**();
  - ポインタを1パラグラフ分後ろに動かします。
- **forwardParagraph**();
  - ポインタを1パラグラフ分前に動かします。
- **pageUp**();
  - ポインタを1ページ分戻します。
- **pageDown**();
  - ポインタを1ページ分進めます。
- **headOfRow**();
  - ポインタを文頭に移動させます。
- **tailOfRow**();
  - ポインタを文末に移動させます。
- **headOfCol**();
  - ポインタを行頭に移動させます。
- **headOfRow**();
  - ポインタを行頭に移動させます。
- **physicalHeadOfCol**();
  - ポインタを見た目上の行頭に移動させます(行が折り返し位置も行頭とみなす)。
- **physicalTailOfCol**();
  - ポインタを見た目上の行末に移動させます(行が折り返し位置も行末とみなす)。
- **splitAtPointer**();
  - ポインタ位置で、行を2つに分割します。
- **backspaceAtPointer**();
  - ポインタ位置手前の1文字を削除します。
- **deleteAtPointer**();
  - ポインタ位置直後の1文字を削除します。
- **pasteAtePointer**();
  - ポインタ位置にペーストします。
- **copySelection**();
  - 選択範囲をコピーします。
- **cutSelection**();
  - 選択範囲をカットします。
- **selectAll**();
  - 全領域を選択範囲にします。
- **markAtPointer**();
  - ポインタ位置をマークします。
- **markAndHeadOfRow**();
  - ポインタ位置をマークして文頭に移動します。
- **markAndTailOfRow**();
  - ポインタ位置をマークして文末に移動します。
- **copyMarkToPointer**();
  - マークとポインタの間をコピーします。
- **cutMarkToPointer**();
  - マークとポインタの間をカットします。
- **killLine**();
  - ポインタから行末までを削除します。
- **undo**();
  - 1動作undoを行います。

## フック
- **onPointerAt**(*row, col*);
  - ポインタが移動した時に呼ばれます。
- **onSplitAt**(*row, col*);
  - 行が分割された時に呼ばれます。
- **onJoinAt**(*row, col();
  - 行が結合された時に呼ばれます。
- **onInsertAt**(*row, col, str*);
  - 文字列が挿入された時に呼ばれます。
- **onDeleteAt**(*row, col, str*);
  - 文字列が削除された時に呼ばれます。


