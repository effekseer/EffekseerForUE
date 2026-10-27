import json
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

import test_ue_builds as target


class BuildTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name).resolve()
        for name in ('Plugin', 'Plugin with spaces'):
            (self.root / name).mkdir()
            (self.root / name / 'Effekseer.uplugin').write_text('{}')
        mock_commit = patch.object(target.subprocess, 'check_output', return_value='test-commit\n')
        mock_commit.start()
        self.addCleanup(mock_commit.stop)

    def engine(self, name, version=(5, 8, 0)):
        root = self.root / name
        build = root / 'Engine/Build'
        (build / 'BatchFiles').mkdir(parents=True)
        (build / 'BatchFiles/RunUAT.bat').write_text('@exit /b 0\n')
        (build / 'Build.version').write_text(json.dumps(dict(zip(
            ('MajorVersion', 'MinorVersion', 'PatchVersion'), version))))
        return root

    def test_discovery_deduplicates_and_ignores_stale_entries(self):
        root = self.engine('UE_5.8')
        (self.root / 'UE_5.7').mkdir()
        engines = target.discover([root, root, self.root / 'missing'], [self.root])
        self.assertEqual([e['version'] for e in engines], ['5.8.0'])

    def test_malformed_version_is_not_a_valid_engine(self):
        root = self.engine('UE_broken')
        (root / 'Engine/Build/Build.version').write_text('{}')
        self.assertEqual(target.discover([root], []), [])

    def test_version_selection_requires_every_requested_version(self):
        engines = target.discover([self.engine('UE_5.8'), self.engine('UE_4.27', (4, 27, 2))], [])
        self.assertEqual(target.select_versions(engines, ['5.8'])[0]['version'], '5.8.0')
        with self.assertRaises(ValueError):
            target.select_versions(engines, ['5.8', '5.7'])

    def test_failed_build_does_not_skip_other_engines(self):
        engines = target.discover([self.engine('UE_5.8'), self.engine('UE_4.27', (4, 27, 2))], [])
        plugin = self.root / 'Plugin/Effekseer.uplugin'
        output = self.root / 'results'
        with patch.object(target.subprocess, 'run', side_effect=[
                subprocess.CompletedProcess([], 7), subprocess.CompletedProcess([], 0)]) as run:
            self.assertEqual(target.run_builds(engines, plugin, output), 1)
        self.assertEqual(run.call_count, 2)
        self.assertIn('Input', run.call_args.args[0][2])
        self.assertFalse((plugin.parent / 'Config').exists())
        summary = json.loads(next(output.glob('*/summary.json')).read_text())
        self.assertEqual([r['exit_code'] for r in summary['results']], [7, 0])
        self.assertEqual([r['status'] for r in summary['results']], ['failed', 'passed'])
        self.assertTrue(all(Path(r['log']).is_file() for r in summary['results']))

    def test_output_never_reuses_an_existing_package(self):
        engines = target.discover([self.engine('UE_5.8')], [])
        output = self.root / 'results'
        with patch.object(target.subprocess, 'run') as run:
            for _ in range(2):
                self.assertEqual(target.run_builds(engines, self.root / 'Plugin/Effekseer.uplugin', output, True), 0)
            run.assert_not_called()
        self.assertEqual(len(list(output.glob('*/summary.json'))), 2)
        with self.assertRaises(ValueError):
            target.run_builds(engines, self.root / 'Plugin/Effekseer.uplugin', Path(engines[0]['root']) / 'output', True)

    @unittest.skipUnless(target.os.name == 'nt', 'Windows batch execution')
    def test_real_batch_execution_with_spaces(self):
        root = self.engine('UE with spaces')
        (root / 'Engine/Build/BatchFiles/RunUAT.bat').write_text('@echo stub UAT output\n@exit /b 7\n')
        engines = target.discover([root], [])
        output = self.root / 'output with spaces'
        self.assertEqual(target.run_builds(engines, self.root / 'Plugin with spaces/Effekseer.uplugin', output), 1)
        summary = json.loads(next(output.glob('*/summary.json')).read_text())
        self.assertEqual(summary['results'][0]['exit_code'], 7)
        self.assertIn('stub UAT output', Path(summary['results'][0]['log']).read_text())

    def test_ue4_tool_path_preserves_process_search_policy(self):
        engine = target.discover([self.engine('UE_4.27', (4, 27, 2))], [])[0]
        with patch.dict(target.os.environ, {'PATH': 'original', 'NoDefaultCurrentDirectoryInExePath': '1'}):
            environment = target.build_environment(engine)
            self.assertEqual({k.upper(): v for k, v in environment.items()}['NODEFAULTCURRENTDIRECTORYINEXEPATH'], '1')
            self.assertEqual(environment['PATH'].split(target.os.pathsep)[0],
                             str(Path(engine['root']) / 'Engine/Binaries/DotNET'))
            self.assertEqual(target.os.environ['PATH'], 'original')

    def test_rejects_batch_metacharacters(self):
        with self.assertRaises(ValueError):
            target.build_command({'root': 'C:/UE%unsafe%'}, Path('Plugin.uplugin'), Path('Output'))


if __name__ == '__main__':
    unittest.main()
