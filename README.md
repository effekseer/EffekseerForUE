EffekseerForUE
=========

This is a wrapper for UnrealEngine.
Effekseer is a tool editing particle effects. By incorporating a library, you will be able to show the effects that was created with Effekseer.

UnrealEngine向けEffekseerのラッパーです。
 Effekseerはパーティクルエフェクト編集ツールです。 ライブラリを組み込むことで、Effekseerで作成されたエフェクトを再生できるようになります。

這是UnrealEngine的Effekseer的包裝。 Effekseer為粒子效果的編輯工具。 透過組合程式庫和插件,由Effekseer制作而成的效果便可以在應用程式內播放。

这是UnrealEngine的Effekseer的包装。 Effekseer是一个粒子效果的编辑工具。 透过组合库和插件,由Effekseer制作而成的效果便可以在应用程式内播放。

How to use
=========

Please read the help below.

下記のヘルプを読んでください。

請閱讀下面的幫助。

请阅读下面的帮助。

[Help](https://effekseer.github.io/Help_UE4/)

UE4.27
=========

A sln cannot be generated without this script.

下記のスクリプトでないとslnを生成できません。

```
set ENGINE_PATH=C:\Path\To\Epic Games\UE_4.27

"%ENGINE_PATH%\Engine\Binaries\DotNET\UnrealBuildTool.exe" -projectfiles -project=C:\Path\To\EffekseerForUE\EffekseerForUE.uproject -game -engine -progress -2019
```