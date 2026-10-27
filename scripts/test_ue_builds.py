"""Discover Windows Unreal Engine installations and compile the Effekseer plugin."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import shutil
import sys
import tempfile
import time

REPO = Path(__file__).resolve().parents[1]


def read_json(path):
    return json.loads(Path(path).read_text(encoding='utf-8-sig'))


def registered_roots():
    launcher = Path(os.environ.get('PROGRAMDATA', 'C:/ProgramData')) / 'Epic/UnrealEngineLauncher/LauncherInstalled.dat'
    if launcher.is_file():
        try:
            for entry in read_json(launcher).get('InstallationList', []):
                if entry.get('InstallLocation'):
                    yield Path(entry['InstallLocation'])
        except (OSError, ValueError, TypeError) as error:
            print(f'Warning: cannot read {launcher}: {error}', file=sys.stderr)
    if os.name != 'nt':
        return
    import winreg
    for view in (winreg.KEY_WOW64_64KEY, winreg.KEY_WOW64_32KEY):
        try:
            with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r'SOFTWARE\EpicGames\Unreal Engine', 0, winreg.KEY_READ | view) as key:
                for i in range(winreg.QueryInfoKey(key)[0]):
                    try:
                        with winreg.OpenKey(key, winreg.EnumKey(key, i)) as version:
                            yield Path(winreg.QueryValueEx(version, 'InstalledDirectory')[0])
                    except OSError:
                        continue
        except OSError:
            pass
    try:
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER, r'SOFTWARE\Epic Games\Unreal Engine\Builds') as key:
            for i in range(winreg.QueryInfoKey(key)[1]):
                yield Path(winreg.EnumValue(key, i)[1])
    except OSError:
        pass


def discover(roots, search_roots):
    candidates = list(roots)
    for parent in search_roots:
        try:
            candidates.extend(Path(parent).glob('UE_*'))
        except OSError as error:
            print(f'Warning: cannot scan {parent}: {error}', file=sys.stderr)
    engines = {}
    for candidate in candidates:
        root = Path(candidate).resolve()
        key = os.path.normcase(str(root))
        if key in engines:
            continue
        uat = root / 'Engine/Build/BatchFiles/RunUAT.bat'
        version_file = root / 'Engine/Build/Build.version'
        if not uat.is_file() or not version_file.is_file():
            continue
        try:
            data = read_json(version_file)
            version = tuple(int(data[name]) for name in ('MajorVersion', 'MinorVersion', 'PatchVersion'))
        except (OSError, ValueError, KeyError, TypeError) as error:
            print(f'Warning: cannot read {version_file}: {error}', file=sys.stderr)
            continue
        engines[key] = {'root': str(root), 'version': '.'.join(map(str, version)), 'version_parts': version}
    return sorted(engines.values(), key=lambda engine: (engine['version_parts'], engine['root']))


def select_versions(engines, requested):
    if not requested:
        return engines
    selected = []
    for version in requested:
        matches = [engine for engine in engines if engine['version'] == version or engine['version'].startswith(version + '.')]
        if not matches:
            raise ValueError(f'No valid UE installation matches {version}')
        selected.extend(engine for engine in matches if engine not in selected)
    return selected


def build_command(engine, plugin, package):
    # RunUAT is a Windows batch file. Reject shell metacharacters in paths;
    # subprocess handles spaces with Windows argument quoting.
    for path in (engine['root'], str(plugin), str(package)):
        if any(character in path for character in '\r\n"%!*?&|<>^'):
            raise ValueError(f'Unsupported character in build path: {path}')
    return [str(Path(engine['root']) / 'Engine/Build/BatchFiles/RunUAT.bat'),
            'BuildPlugin', f'-Plugin={plugin}', f'-Package={package}',
            '-TargetPlatforms=Win64', '-Unattended', '-UTF8Output']


def build_environment(engine):
    environment = os.environ.copy()
    if engine['version_parts'][0] == 4:
        # UE4 RunUAT invokes AutomationToolLauncher.exe without a path. Keep
        # NoDefaultCurrentDirectoryInExePath intact and name its directory.
        tools_dir = Path(engine['root']) / 'Engine/Binaries/DotNET'
        environment['PATH'] = str(tools_dir) + os.pathsep + environment.get('PATH', '')
    return environment


def run_builds(engines, plugin, output_root, dry_run=False):
    output_root = Path(output_root).resolve()
    # BuildPlugin deletes existing package output. Never allow output within the
    # plugin source or an engine, and always allocate a fresh run directory.
    for source in [plugin.parent] + [Path(engine['root']) for engine in engines]:
        if output_root == source or source in output_root.parents:
            raise ValueError(f'Output must be outside plugin/engine source: {output_root}')
    output_root.mkdir(parents=True, exist_ok=True)
    run_dir = Path(tempfile.mkdtemp(prefix=time.strftime('%Y%m%d-%H%M%S-'), dir=output_root))
    build_plugin = run_dir / 'Input' / plugin.parent.name / plugin.name
    if not dry_run:
        # UAT may create Config/FilterPlugin.ini in its input plugin directory.
        shutil.copytree(plugin.parent, build_plugin.parent,
                        ignore=shutil.ignore_patterns('Intermediate', 'Saved', '.vs', '.git'))
    summary = {'plugin': str(plugin), 'build_plugin': str(build_plugin), 'dry_run': dry_run, 'results': []}
    try:
        summary['commit'] = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=REPO, text=True, stderr=subprocess.DEVNULL).strip()
    except (OSError, subprocess.CalledProcessError):
        summary['commit'] = None
    print(f'Results: {run_dir}', flush=True)
    for index, engine in enumerate(engines):
        destination = run_dir / f"{index + 1:02d}-UE-{engine['version']}"
        destination.mkdir()
        command = build_command(engine, build_plugin, destination / 'Package')
        result = dict(engine, command=command, log=str(destination / 'build.log'), status='planned', exit_code=None)
        summary['results'].append(result)
        start = time.monotonic()
        print(f"UE {engine['version']}: {engine['root']}", flush=True)
        print(subprocess.list2cmdline(command), flush=True)
        try:
            if not dry_run:
                with open(result['log'], 'w', encoding='utf-8') as log:
                    completed = subprocess.run(command, cwd=REPO, stdout=log, stderr=subprocess.STDOUT, check=False, env=build_environment(engine))
                result['exit_code'] = completed.returncode
                result['status'] = 'passed' if completed.returncode == 0 else 'failed'
        except OSError as error:
            result.update(status='failed', error=str(error))
        except KeyboardInterrupt:
            result['status'] = 'interrupted'
            raise
        finally:
            result['seconds'] = round(time.monotonic() - start, 2)
            (run_dir / 'summary.json').write_text(json.dumps(summary, indent=2, ensure_ascii=False), encoding='utf-8')
        print(f"  {result['status']} ({result['seconds']}s)", flush=True)
    return 1 if any(result['status'] == 'failed' for result in summary['results']) else 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--list', action='store_true', help='List valid engines without building')
    parser.add_argument('--dry-run', action='store_true', help='Save commands without running UAT')
    parser.add_argument('--engine-root', action='append', default=[], help='Explicit engine path; disables automatic registration discovery')
    parser.add_argument('--search-root', action='append', default=[], help='Additional parent directory containing UE_* folders')
    parser.add_argument('--engine-version', action='append', default=[], help='Select a major.minor or exact version; repeatable')
    parser.add_argument('--output-root', type=Path, default=REPO / 'Saved/BuildPluginTests')
    args = parser.parse_args(argv)
    roots = args.engine_root or list(registered_roots())
    search_roots = list(args.search_root)
    if not args.engine_root:
        for name in ('ProgramFiles', 'ProgramFiles(x86)'):
            if os.environ.get(name):
                search_roots.append(Path(os.environ[name]) / 'Epic Games')
    engines = select_versions(discover(roots, search_roots), args.engine_version)
    if not engines:
        raise ValueError('No valid UE installations found. Use --search-root or --engine-root.')
    if args.list:
        for engine in engines:
            print(f"{engine['version']}\t{engine['root']}")
        return 0
    if os.name != 'nt' and not args.dry_run:
        raise ValueError('Build execution requires Windows.')
    plugin = (REPO / 'Plugins/Effekseer/Effekseer.uplugin').resolve()
    if not plugin.is_file():
        raise ValueError(f'Plugin not found: {plugin}')
    return run_builds(engines, plugin, args.output_root, args.dry_run)


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (ValueError, OSError) as error:
        print(f'Error: {error}', file=sys.stderr)
        sys.exit(1)
    except KeyboardInterrupt:
        sys.exit(130)
