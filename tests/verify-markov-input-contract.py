#!/usr/bin/env python3
"""Check token cleanup, state windows, negative parameters and original demos."""
from pathlib import Path
import argparse
import json
import os
import platform
import signal
import subprocess
import tempfile
import time
import re

parser = argparse.ArgumentParser()
parser.add_argument("--compiler", default=os.environ.get("CXX", "c++"))
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
task = os.environ.get("CLASSES_FAMILY_TASK_ID", "dsa-markov-input-contract")

def stop(signum, _frame):
    raise SystemExit(128 + signum)


for signum in (signal.SIGTERM, signal.SIGHUP, signal.SIGINT):
    signal.signal(signum, stop)


def run(command, expected=None, env=None, exit_code=0):
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
    if child.returncode != exit_code or (expected is not None and out != expected):
        raise RuntimeError(f'Failed command: {command}\nstdout:\n{out}\nstderr:\n{err}')
    if err and exit_code == 0:
        raise RuntimeError(f'Unexpected diagnostic from {command}:\n{err}')
    return out



flags = ["-std=c++20", "-Wall", "-Wextra", "-Wpedantic", "-Werror"]
reports = []
with tempfile.TemporaryDirectory(prefix="dsa-markov-contract-") as temporary:
    work = Path(temporary)
    for role in ["starter", "solution"]:
        source = root / "DSCPP3-Markov-Text-Generator" / role / "main.cpp"
        original = source.read_bytes()
        assert original.count(b"int main()") == 1
        if role == "starter":
            assert original.count(b"TODO:") == 1
            assert b"TODO: replace the preview with a state-based text generator." in original
        (work / "markov-under-test.hpp").write_bytes(original[:original.index(b"int main()")])
        for mode in ["ordinary", "sanitized"]:
            mode_flags = ["-O2"] if mode == "ordinary" else ["-O1", "-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer"]
            environment = dict(os.environ)
            environment["ASAN_OPTIONS"] = ("detect_leaks=1" if platform.system() == "Linux" else "detect_leaks=0") + ":halt_on_error=1"
            environment["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
            binary = work / "markov-checks"
            run([args.compiler, *flags, *mode_flags, '-I'+str(work), '-DMARKOV_REFERENCE='+str(int(role == "solution")), str(root / "tests/markov-input-contract.cpp"), '-o', str(binary)])
            expected = "Token cleanup and unfinished preview passed\n" if role == "starter" else "Token cleanup, 320 window models and input guards passed\n"
            run([str(binary)], expected=expected, env=environment)
            demonstration = work / "markov-demo"
            run([args.compiler, *flags, *mode_flags, str(source), '-o', str(demonstration)])
            output = run([str(demonstration)], env=environment)
            if role == "starter":
                assert output == "Token count: 11\nUnique count: 9\nStarter preview: structures matter trees matter lists matter\nTODO: replace the preview with a state-based text generator.\n", output
            else:
                assert output.startswith("Token count: 22\nUnique count: 16\nGenerated:"), output
                assert output == run([str(demonstration)], env=environment)
                assert output.count('\n') == 3
                baseline = work / "original-reference.cpp"
                baseline.write_bytes(subprocess.check_output(['git','show','1f9c14d8a13072688324e8f97c339d02115ece1a:DSCPP3-Markov-Text-Generator/solution/main.cpp'],cwd=root))
                original_demo = work / "original-reference-demo"
                baseline_flags = flags
                run([args.compiler,*baseline_flags,*mode_flags,str(baseline),'-o',str(original_demo)])
                assert output == run([str(original_demo)],env=environment)
            reports.append({'role':role,'mode':mode,'windowModels':320 if role=='solution' else 0,'classBodiesPreserved':True,'negativeParametersRejected':role=='solution','zeroOrderPreserved':role=='solution','originalDemonstrationVerified':True,'unfinishedGeneratorPreserved':role=='starter'})
    print(json.dumps({'event':'verified-markov-input-contract','parentTaskId':task,'compiler':args.compiler,'nativeRoleModeGroups':len(reports),'roles':reports,'scope':'Reference input/state contract and untouched starter preview; no completed learner generator acceptance'}), flush=True)
