#!/usr/bin/env python3
"""Check quicksort assignment roles and independent algorithm properties."""
from pathlib import Path
import argparse
import json
import hashlib
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
task = os.environ.get("CLASSES_FAMILY_TASK_ID", "dsa-quicksort-contract")

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
with tempfile.TemporaryDirectory(prefix="dsa-quicksort-contract-") as temporary:
    work = Path(temporary)
    for role in ["starter", "solution"]:
        source = root / "DSCPP5-Quicksort-Toolkit" / role / "main.cpp"
        original = source.read_bytes()
        assert original.count(b"int main()") == 1
        if role == "starter":
            assert original.count(b"TODO:") == 3
            assert b"std::sort" not in original
            assert b"quicksort pending" in original
        else:
            assert hashlib.sha256(original.replace(b"\r\n", b"\n")).hexdigest() == "9b699d7b140ce53a9956f19410a09a33c83869d8bc3766e3f1d87b3c63df4523"
        (work / "quicksort-under-test.hpp").write_bytes(original[:original.index(b"int main()")])
        for mode in ["ordinary", "sanitized"]:
            mode_flags = ["-O2"] if mode == "ordinary" else ["-O1", "-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer"]
            environment = dict(os.environ)
            environment["ASAN_OPTIONS"] = ("detect_leaks=1" if platform.system() == "Linux" else "detect_leaks=0") + ":halt_on_error=1"
            environment["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
            binary = work / "quicksort-checks"
            run([args.compiler, *flags, *mode_flags, "-I"+str(work), "-DQUICKSORT_REFERENCE="+str(int(role == "solution")), str(root/"tests/quicksort-contract.cpp"), "-o", str(binary)])
            output = json.loads(run([str(binary)], env=environment))
            assert output['role'] == role
            if role == 'solution':
                assert output['sortCases'] == 3816
                assert output['partitionCases'] == 516
                assert output['pivotCases'] == 516
            else:
                assert output['unfinishedCases'] == 4
            demo = work / "quicksort-demo"
            run([args.compiler, *flags, *mode_flags, str(source), "-o", str(demo)])
            expected = "Starter values (quicksort pending): 7, 2, 9, 4, 1, 8\n" if role == 'starter' else "Sorted values: 1, 2, 4, 7, 8, 9\n"
            run([str(demo)], expected=expected, env=environment)
            reports.append(dict(output, mode=mode, demonstrationVerified=True))
    print(json.dumps({'event':'verified-quicksort-contract', 'parentTaskId':task, 'compiler':args.compiler, 'nativeRoleModeGroups':len(reports), 'roles':reports, 'referenceBytesPreserved':True, 'scope':'Quicksort source roles, bounded valid-index pivot/partition properties, independent sort oracle and original reference demonstration; no browser or full-course acceptance'}), flush=True)
