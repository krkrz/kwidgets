# KHorizontalBarLayout

KHorizontalBarLayout は登録した順番に横方向にウィジェットを配置するレイアウトウィジェットです。

**KHorizontalBarLayout** -> [KGridLayout](KGridLayout.md) -> [KLayout](KLayout.md) -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## コンストラクタ
```KHorizontalBarLayout(window, options = %[])```

## スタイル
- **borderStyle**, **paddingStyle**
  - スタイル指定に基づき、子ウィジェットの周囲にボーダーが描画されます。
- **layoutStyle**
  - スタイル指定に基づき、子ウィジェット間に任意のスペースを挿入します。

## メソッド
- **add**(*child*);
  - *child*を追加します。
 
- **remove**(*child, doInvalidate = true*);
  - 指定の子ウィジェット*child*を削除します。

	デフォルト動作では削除時に*child*が自動的に無効化されますが、
	*doInvalidate*にfalseを指定することで
	removeした*child*を再利用することができます。

