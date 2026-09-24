"""Regression: a failed test build must not run a stale passing test binary."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

repo = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='myeda-build-check-') as temp:
    root = Path(temp)
    shutil.copyfile(repo / 'build.ps1', root / 'build.ps1')
    debug = root / 'MyEDA/build/Debug'
    debug.mkdir(parents=True)
    shutil.copyfile(repo / 'MyEDA/build/Debug/test_sim.exe', debug / 'test_sim.exe')
    wx = root / 'wx/lib/vc_x64_lib'
    wx.mkdir(parents=True)
    (root / 'cmake.cmd').write_text('@echo off\nfor %%a in (%*) do if "%%a"=="test_sim" exit /b 9\nexit /b 0\n', encoding='ascii')
    env = os.environ.copy()
    env['PATH'] = str(root) + os.pathsep + env['PATH']
    result = subprocess.run(['powershell.exe', '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', str(root / 'build.ps1'), '-Wx', str(root / 'wx')], env=env, capture_output=True)
    print('Exit code:', result.returncode)
    print('Stale tests ran:', b'[PASS]' in result.stdout)
    assert result.returncode != 0, 'Test compilation failed but build script returned success'
    assert b'[PASS]' not in result.stdout, 'Script ran stale tests after their compilation failed'
    print('PASS: failed test compilation stops build')
