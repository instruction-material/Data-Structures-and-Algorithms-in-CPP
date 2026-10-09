#!/usr/bin/env python3
"""Check copy rejection and actual object lifetimes in the teaching collections."""
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
task = os.environ.get("CLASSES_FAMILY_TASK_ID", "dsa-node-ownership")

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


flags = ["-std=c++20", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wsign-conversion", "-Werror"]
roles = [("DSCPP6-Template-Linked-List", role, 1) for role in ["starter", "solution"]]
roles += [("DSCPP7-Binary-Search-Tree", role, 2) for role in ["starter", "solution"]]
roles += [("DSCPP8-AVL-Tree", role, 3) for role in ["starter", "solution"]]
roles += [("DSCPP9-Performance-Benchmarks", "solution", 4)]
reports = []
with tempfile.TemporaryDirectory(prefix="dsa-owner-native-") as temporary:
    work = Path(temporary)
    for mode in ["ordinary", "sanitized"]:
        mode_flags = ["-O2"] if mode == "ordinary" else [
            "-O1", "-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=all",
            "-fno-omit-frame-pointer"]
        environment = dict(os.environ)
        environment["ASAN_OPTIONS"] = ("detect_leaks=1" if platform.system() == "Linux"
                                        else "detect_leaks=0") + ":halt_on_error=1"
        environment["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
        for folder, role, kind in roles:
            source = root / folder / role / "main.cpp"
            binary = work / f"{folder}-{role}-{mode}"
            run([args.compiler, *flags, *mode_flags, '-DCOURSE_SOURCE="' + str(source) + '"',
                 '-DOWNER_KIND=' + str(kind), '-DOWNER_REFERENCE=' + str(int(role == "solution")),
                 str(root / "tests/node-ownership.cpp"), "-o", str(binary)])
            run([str(binary)], expected="Single-owner policy and independent lifetimes passed\n", env=environment)
            demonstration = work / f"{folder}-{role}-{mode}-demo"
            run([args.compiler, *flags, *mode_flags, str(source), "-o", str(demonstration)])
            output = run([str(demonstration)], env=environment)
            assert output
            if kind == 4:
                assert len(output.splitlines()) == 6
                assert all(re.fullmatch(r"(?:vector|set|unordered_set|linked list|bst|avl) insert \(us\): \d+", line)
                           for line in output.splitlines())
            reports.append({"folder": folder, "role": role, "mode": mode,
                            "copyAndMoveRejected": True, "independentLifetimes": 32,
                            "actualDemonstrationRan": True, "passed": True})
print(json.dumps({"event": "verified-node-ownership", "compiler": args.compiler,
                  "reports": reports, "singleOwnerClasses": 9, "actualSourceFiles": 7,
                  "scope": "Copy/move interface and bounded lifetimes only; full coursework semantics remain outside this gate"}))
