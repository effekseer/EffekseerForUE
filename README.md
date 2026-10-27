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

Local build verification (Windows)
==================================

Python 3.9+ and the C++ toolchain/Windows SDK required by each installed UE
version are needed. No additional Python packages are required.

```powershell
# Discover engines without compiling.
python scripts/test_ue_builds.py --list

# Also scan a custom installation directory.
python scripts/test_ue_builds.py --list --search-root "E:\Program Files\Epic Games"

# Print and save the build commands without executing them.
python scripts/test_ue_builds.py --dry-run

# Build against every detected engine, sequentially.
python scripts/test_ue_builds.py

# Select versions (repeat the option to select several).
python scripts/test_ue_builds.py --engine-version 4.27 --engine-version 5.8

# Use only an explicitly specified installation, including a source build.
python scripts/test_ue_builds.py --engine-root "E:\Program Files\Epic Games\UE_5.8"
```

The script reads Epic Games Launcher installation metadata, Windows registry
entries (including per-user source builds), and standard installation directories.
It verifies `Engine/Build/Build.version` and `RunUAT.bat`, removes duplicate paths,
and ignores stale registrations. Custom unregistered locations can be added with
`--search-root` or `--engine-root`. The latter disables automatic discovery.

`RunUAT BuildPlugin -TargetPlatforms=Win64` checks the editor host and supported
Win64 game configurations. This is a compilation/package check, not a rendering,
playback, or sample-game cooking test. Toolchain failures are reported as failures;
the script does not install or modify Visual Studio or Unreal Engine.

UAT receives a working copy of the plugin, so its generated configuration does
not modify the source plugin. UE4 tool paths are supplied to the child process
without changing the system PATH or executable-search security policy.

Each invocation uses a fresh directory under `Saved/BuildPluginTests/` (already
ignored by Git). Each engine gets a `build.log` and `Package/`; the run directory
contains `summary.json` with engine paths, versions, command arguments, repository
commit, exit codes, and elapsed times. Build output is written to the log while
running. Use `Get-Content <build.log> -Wait` in another terminal to follow it.
UAT may also write its normal caches and logs outside this directory.

A failed build does not prevent subsequent engines from being tested. Exit status
is 0 when all builds pass, 1 on failure/missing requested engines, or 130 when
interrupted. `--list` and `--dry-run` do not establish build success. Output can be
relocated with `--output-root`; previous results are never overwritten. Paths with
Windows batch metacharacters are rejected.

Tests for the discovery and execution logic:

```powershell
python -B -m unittest discover -s scripts -p test_ue_builds_tests.py -v
```
