#!/usr/bin/env python3
"""Build and exercise actual DSCPP2 roles without retaining generated binaries."""
from pathlib import Path
import argparse
import json
import os
import platform
import signal
import subprocess
import tempfile
import time

parser = argparse.ArgumentParser()
parser.add_argument('--compiler', default=os.environ.get('CXX', 'c++'))
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
task = os.environ.get('CLASSES_FAMILY_TASK_ID', 'dsa-graph-navigation')


def stop(signum, _frame):
    raise SystemExit(128 + signum)


for signum in (signal.SIGTERM, signal.SIGHUP, signal.SIGINT):
    signal.signal(signum, stop)


def run(command, expected=None, env=None):
    started = time.time()
    child = subprocess.Popen(command, cwd=root, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, text=True,
                             start_new_session=True, env=env)
    print(json.dumps({'event': 'start', 'parentTaskId': task, 'cwd': str(root),
                      'parentPid': os.getpid(), 'pid': child.pid, 'command': command,
                      'startTime': started, 'timeoutSeconds': 90}), flush=True)
    try:
        out, err = child.communicate(timeout=90)
    except BaseException as failure:
        if isinstance(failure, subprocess.TimeoutExpired):
            print(json.dumps({'event': 'timeout', 'parentTaskId': task,
                              'pid': child.pid, 'timeoutSeconds': 90}), flush=True)
        if child.poll() is None:
            os.killpg(child.pid, signal.SIGTERM)
            try:
                child.communicate(timeout=5)
            except subprocess.TimeoutExpired:
                os.killpg(child.pid, signal.SIGKILL)
                child.communicate()
            print(json.dumps({'event': 'child-process-group-cleanup',
                              'parentTaskId': task, 'pid': child.pid}), flush=True)
        raise
    print(json.dumps({'event': 'end', 'parentTaskId': task, 'pid': child.pid,
                      'endTime': time.time(), 'exitCode': child.returncode}), flush=True)
    if child.returncode or (expected is not None and out != expected):
        raise RuntimeError(f'Failed command: {command}\nstdout:\n{out}\nstderr:\n{err}')
    if err:
        raise RuntimeError(f'Unexpected diagnostic from {command}:\n{err}')
    return out


flags = ['-std=c++20', '-Wall', '-Wextra', '-Wpedantic', '-Wconversion',
         '-Wsign-conversion', '-Werror']
reports = []
with tempfile.TemporaryDirectory(prefix='dsa-graph-native-') as temporary:
    work = Path(temporary)
    for mode in ['ordinary', 'sanitized']:
        mode_flags = ['-O2'] if mode == 'ordinary' else [
            '-O1', '-g', '-fsanitize=address,undefined',
            '-fno-sanitize-recover=all', '-fno-omit-frame-pointer']
        environment = dict(os.environ)
        environment['ASAN_OPTIONS'] = ('detect_leaks=1' if platform.system() == 'Linux'
                                       else 'detect_leaks=0') + ':halt_on_error=1'
        environment['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
        for role in ['starter', 'solution']:
            source = root / 'DSCPP2-Graph-Navigation' / role / 'main.cpp'
            sample = work / f'{role}-{mode}-sample'
            check = work / f'{role}-{mode}-checks'
            run([args.compiler, *flags, *mode_flags, str(source), '-o', str(sample)])
            expected = ('Starter path: 0 2 3 4\n' if role == 'starter' else
                        'Shortest path: 0 2 3 4\nCost: 7\n')
            run([str(sample)], expected=expected, env=environment)
            run([args.compiler, *flags, *mode_flags, f'-DCOURSE_SOURCE="{source}"',
                 str(root / 'tests/graph-navigation.cpp'), '-o', str(check)])
            result = run([str(check)], env=environment)
            assert result.startswith('Graph regressions passed: 14 malformed loads, bad stream, ')
            reports.append({'role': role, 'mode': mode, 'malformedLoads': 14,
                            'independentGraphs': 32, 'endpointComparisons': 816,
                            'maximumGraphNodes': 256, 'passed': True})
    cmake = work / 'cmake'
    run(['cmake', '-S', str(root), '-B', str(cmake),
         '-DCMAKE_CXX_COMPILER=' + args.compiler,
         '-DCMAKE_CXX_FLAGS=' + ' '.join(flags)])
    run(['cmake', '--build', str(cmake), '--target', 'dscpp2_starter', 'dscpp2_solution',
         '--parallel', '2'])
    run([str(cmake / 'dscpp2_starter')], expected='Starter path: 0 2 3 4\n')
    run([str(cmake / 'dscpp2_solution')], expected='Shortest path: 0 2 3 4\nCost: 7\n')
print(json.dumps({'event': 'verified-graph-navigation', 'parentTaskId': task,
                  'compiler': args.compiler, 'reports': reports, 'cmakeTargets': 2,
                  'scope': 'DSCPP2 only; other course projects remain unaudited'}))
