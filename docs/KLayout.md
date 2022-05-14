# KLayout

KLayout は「子」を配置することができる「レイアウトウィジェット」のベースとなるクラスです。


## 親クラス

**KLayout** -> [KEntity](KEntity.md) -> [KWidget](KWidget.md)

## コンストラクタ
```KLayout(window, options = %[])```

## フック
- **onCalculateLayout**();
  - レイアウトの再計算を行うタイミングで呼び出されます。

	継承クラスで適切に実装する必要があります。

	子のサイズが変わる、自分自身のスタイルが変わる、
	などの「子ウィジェットのレイアウトを再計算する必要がある」タイミングで呼び出されます。

- **onLocateLayout**() ;
  - 子の再配置を行います。

	継承クラスで適切に実装する必要があります。

	自分のサイズが変わるなど、子の再配置を行う必要があるタイミングで呼び出されます。
	事前に **onCalculateLayout**() で計算しておいたレイアウト情報に基づいて、
	子の再配置を行ってください。
