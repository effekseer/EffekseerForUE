% EffekseerForUnrealEngine4

<div class="main">

## 概要

これはUnrealEngine4向けのEffekseerプラグインです。
このプラグインを使用すると、UnrealEngine4上でEffekseerで作成したエフェクトを再生できるようになります。

## 重要なお知らせ

1.4からアップグレードした場合は、モデルファイルを再インポートしてください。

### 動作環境

#### バージョン

UnrealEngine4.23以降

#### コンパイラ

環境に合わせたコンパイラをインストールする必要があります。
例えば、WindowsではVisualStudio2017、もしくはVisualStudio2019をインストールする必要があります。

#### プラットフォーム

プラットフォーム固有の機能は使用していないため、
EffekseerForUnrealEngine4はほとんどのプラットフォームで動作します。

## 構成

EffekseerForUE4では主に2つのコンポーネントで構成されています。

それは、```EffekseerSystemComponent``` と ```EffekseerEmitterComponent``` です。

EffekseerSystemはエフェクトの描画を行います。そのため、レベル内に必ず1つ必要です。

EffekseerEmitterComponentはエフェクトの生成や操作を行います。エフェクト1つ1つに必要です。

これらの他に、Effekseerのefkefcファイルに対応するアセットの```EffekseerEffect```と
Effekseerのefkmdlファイルに対応するアセットの```EffekseerModel```、efkmatファイルに対応するアセットの```EffekseerMaterial```、があります。

また、コンポーネントを使用しやすくした、```BP_EffekseerSystem``` と ```BP_EffekseerEmitter``` のブループリントもあります。

## サンプル

``` EffekseerForUE4.uproject ``` にサンプルのレベルが収録されています。

これを見るとEffekseerのエフェクトが再生されていることがわかります。


<div align="center">
<img src="img/ss.png">
</div>


## インストール・使用方法

ここでは、サンプル以外のプロジェクトでEffekseerForUE4をインストール・使用する方法について説明します。

### 1.1. ディレクトリのコピー

```Plugins``` のディレクトリをあなたが作成したuprojectと同じディレクトリにコピーします。

<div align="center">
<img src="img/copyFiles.png">
</div>

### 1.2. C++のコードの追加

中身は空で問題ないので、何かしらのC++のコードをプロジェクトに追加します。

<div align="center">
<img src="img/newCpp.png">
</div>

<div align="center">
<img src="img/classNone.png">
</div>


### 1.3. Plugin の有効化

UnrealEngine4からPluginを有効化します。再起動を求められる場合があるので、UnrealEngine4を再起動します。

<div align="center">
<img src="img/enabledPlugin.png">
</div>

再起動した後にコンパイルを求められることがあります。プロジェクトをコンパイルします。

<div align="center">
<img src="img/recompile.png">
</div>

### 1.4. ブループリントの追加

```BP_EffekseerSystem``` と ```BP_EffekseerEmitter``` をレベルに追加します。

これらはプラグインのコンテンツにあるので、プラグインのコンテンツを表示できるようにします。

<div align="center">
<img src="img/showPluginContent.png">
</div>

プラグインのコンテンツのディレクトリに移動します。

<div align="center">
<img src="img/moveDirectory.png">
</div>

ブループリントをレベルに追加します。

<div align="center">
<img src="img/blueprints.png">
</div>

現在、BP_EffekseerSystemは必ず、(0,0,0)の位置に配置する必要があります。

<div align="center">
<img src="img/effekseerSystem.png">
</div>

BP_EffekseerEmitterはエフェクトを表示したい位置に配置します。

<div align="center">
<img src="img/effekseerEmitter.png">
</div>

### 1.5. アセットの追加

efkefcファイルとテクスチャをコンテンツに追加します。

<div align="center">
<img src="img/addAssets.png">
</div>

#### インポートの順番について

インポートの順番は重要です。
テクスチャ、マテリアル、エフェクト、の順番にインポートすることを推奨しています。
この順番でインポートしない場合、リソースが割り当てられず表示がおかしくなることがあります。

エフェクトにテクスチャやがマテリアルが割り当てられていない場合、EffekseerEffectアセットを右クリックした後、AssignResourcesを実行します。

<div align="center">
<img src="img/assignResource.png">
</div>

マテリアルにテクスチャが割り当てられていない場合、EffekseerMaterialアセットを右クリックした後、再インポートを実行します。


エフェクトが小さいことが多いので、EffekseerEffectアセットをダブルクリックしてスケールを設定します。
大きい数字を入力してエフェクトを見やすくしましょう。

<div align="center">
<img src="img/changeScale.png">
</div>

BP_EffekseerEmitterを選択して、コンポーネントを表示させます。
EffekseerEmitterコンポーネントのエフェクトに、先ほど読み込んだEffekseerEffectアセットを設定します。

<div align="center">
<img src="img/setEffect.png">
</div>

EffekseerEmitterコンポーネントのAuto ActivateをOnにします。

<div align="center">
<img src="img/activate.png">
</div>

実行すると、エフェクトが表示されるのがわかります。

<div align="center">
<img src="img/play.png">
</div>

### 2.1. Effekseerのコンポーネント

他のアクターにEffekseerのコンポーネントを追加することもできます。
ただし、EffekseerSystemは複雑なので、EffekseerEmitterのコンポーネントのみ使用することをお勧めします。

<div align="center">
<img src="img/addComponent.png">
</div>

### 3.1. アニメーション通知

アニメーション通知でも使用できます。アニメーションシーケンス内で指定した時間にエフェクトを再生できます。

<div align="center">
<img src="img/animNotify.png">
</div>

### 4.1 ネットワーク機能

* Windowsのみ

ネットワーク経由でアプリケーション(ゲーム等)で再生しているエフェクトを外部からアプリケーションの起動中に編集することができます。

<img src="img/network.png" width="800">

EffekseerSystemコンポーネントにEffekseerから接続するためのポートを指定します。DoStartNetworkAutomaticallyをOnにするか、EffekseerSystemコンポーネントのStartNetworkを実行します。
そうすると、Effekseerからエフェクトを編集できるようになります。他のコンピューターからエフェクトを編集するためにはファイヤーウォールの設定でポート開放する必要があります。

<img src="img/network_ui.png">

制限として、ブレンドの変更や使用していない画像への変更は適用されません。

## 注意

α付きのグレースケールの画像は表示がおかしくなります。

## 制限

減算、歪みはEffekseerでの表示と異なります。

モデルへの法線マップは適用されません。

モデルを大量に描画すると速度が低下しやすくなります。

カリングの設定に対応していません。

マテリアルは深度を無視する設定が適用されません。

## Todo

- 減算、歪みのEffekseerとUnrealEngine4の表示の一致

- モデルの法線の実装

- 使いやすさの向上

## 更新履歴

### 1.52m or 以降

これを読んでください。

[リリースノート](https://effekseer.github.io/Help_Tool/ja/releasenotes.html)

### 1.52k (2020/08/13)

- 画像参照ノードのRとGが入れ替わっていた不具合を修正
- 特定の状況でマテリアルのコンパイルエラーが発生する不具合を修正
- ライティングのパラメーターに対応していなかった不具合を修正
- 再インポートできなかった不具合の修正
- マテリアルで法線の画像を読み込めなかった不具合の修正
- マテリアルのラップとリピートに対応
- マテリアル内のサンプラーをサポートしてなかった不具合を修正
- VertexColorNodeがVector4を返していた不具合を修正
- Effekseer1.52kに更新

### 1.52j (2020/08/02)

- Effekseer1.52jに更新

### 1.52i (2020/07/30)

- Effekseer1.52iに更新

### 1.52h (2020/07/08)

- Effekseer1.52hに更新
- ステップノードの振る舞いがおかしい不具合を修正

### 1.52g (2020/06/09)

- 4.25に対応
- Effekseer1.52gに更新

### 1.52b (2020/05/02)

- 生成方法の線だとパーティクルが表示されない不具合を修正

### 1.52a (2020/04/28) 

- 約90度に回転したZ軸回転ビルボードの挙動がおかしい不具合を修正

### 1.52 (2020/04/26) 

- 4.25 Preview7に対応
- 稀にクラッシュする不具合を修正

### 1.50 (2020/03/18)

- 4.24に対応
- マテリアルに部分的に対応
- デフォルトのパラメーターを変更 (AutoActicate, Scale)

### 1.43f (2019/09/16) 

- 4.23に対応

### 1.43e (2019/07/15) 

- 4.21でコンパイルエラーが発生した不具合を修正

### 1.43d (2019/07/11) 

- 4.22でモデルの描画でクラッシュする不具合を修正

### 1.43b (2019/05/01) 

- 4.22に対応

### 1.43 (2018/03/02) 

- 4.21に対応

- ネットワーク機能の追加

### 1.40 (2018/09/12) 

- 4.20に対応

- 関数の追加(再生速度、色の設定)

### 1.31 (2017/11/20) 

-複数の不具合を修正

-アニメーション通知に対応

### 1.30 (2017/11/01)

-公開

## ライセンス

<pre>

The MIT License (MIT)

Copyright (c) 2011 Effekseer Project

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</pre>

</div>



