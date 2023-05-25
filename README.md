# Direct2DPhotoViewer

C++/WindowsAPI/Direct2Dの勉強として、操作のシンプルさを目指したフォトビューアを制作中です。

Visual Studioでインストールできるライブラリ以外の使用は禁止という縛りの基で作成。

![スクリーン書っと](./Screenshot.png)

--- 

## キーボード
|キー|説明|実装状況|
|--|--|--|
|WASD|ウィンドウの移動|✅|
|QE|画像のサイズ変更|✅|
|Ctrl+WASD|画像の移動|✅|
|Ctrl+QE|画像の回転|✅|
|R|画像の移動・サイズ・回転リセット|✅|
|←→|透明度の変更|✅|
|↑↓|ウィンドウサイズの変更|✅|
|B|背景の変更（黒→白→透明→...のループ）|✅|
|L|描画方法の変更（線形・ニアレストネイバー）|✅|
|O|新しい画像を開く|🔄️|
|N|ビューアの複製|✅|
|T|最前面への配置の有効・無効切り替え|✅|
|?|操作ヘルプ|📝|
|(未定)|簡易的な色調整|📝|
|Esc|ウィンドウを閉じる|✅|

~~Shift同時押しで値変更の調整スピードが上がる~~（未実装）

---
## マウス
|操作|説明|実装状況|
|--|--|--|
|左ドラッグ|ウィンドウの移動|✅|
|右ドラッグ|画像のサイズ変更|🔄️|
|Ctrl+左ドラッグ|画像の移動|🔄️|
|Ctrl+右ドラッグ|画像の回転|🔄️|
|中央クリック|画像の移動・サイズ・回転リセット|✅|
|ホイール回転|画像のサイズ変更|⚡|

---
## TODO
|内容|実装状況|
|--|--|
|ソースコードを綺麗に纏める|🔄️（随時）|
|ソースコードを最適化|🔄️|
|読込失敗時の例外処理追加|🔄️|
|画像の変更ができるようにする|🔄️|
|透過部分はウィンドウイベントを透過|⚠️|
|ウィンドウを複製|✅|

---
### マーク
|アイコン|説明|
|--|--|
|✅|済み|
|🔄️|更新中|
|⚠️|問題あり（停滞中）|
|⚡|バグ発見|
|❌|頓挫|
|📝|追加予定|

---
## 参考
Microsoft Learn: https://learn.microsoft.com/ja-jp/windows/win32/Direct2D/direct2d-overview

## 素材
サンプル画像: https://en.wikipedia.org/wiki/File:PNG_transparency_demonstration_1.png