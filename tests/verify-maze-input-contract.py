#!/usr/bin/env python3
"""Check exact maze input, preserved state, native paths and original demos."""
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
task = os.environ.get("CLASSES_FAMILY_TASK_ID", "dsa-maze-input-contract")

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
solver_driver_hashes = {'starter': '8f8e56df97989b6d42020b7a8d4400c4002ee60720f141046041027a4ed318ef', 'solution': '66b2923fbf514f8abc2eb15c97456c1bc05350791e851da3af717e19af0538b1'}
reports = []
with tempfile.TemporaryDirectory(prefix="dsa-maze-contract-") as temporary:
    work = Path(temporary)
    for role in ["starter", "solution"]:
        source = root / "DSCPP4-Recursive-Maze-Pathfinder" / role / "main.cpp"
        original = source.read_bytes()
        text = source.read_text()
        assert original.count(b"int main()") == 1
        assert hashlib.sha256(text[text.index("    std::string toString"):].encode()).hexdigest() == solver_driver_hashes[role]
        if role == "starter":
            assert original.count(b"TODO:") == 1
            assert b"TODO: implement recursive backtracking" in original
        (work / "maze-under-test.hpp").write_bytes(original[:original.index(b"int main()")])
        for mode in ["ordinary", "sanitized"]:
            mode_flags = ["-O2"] if mode == "ordinary" else ["-O1", "-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer"]
            environment = dict(os.environ)
            environment["ASAN_OPTIONS"] = ("detect_leaks=1" if platform.system() == "Linux" else "detect_leaks=0") + ":halt_on_error=1"
            environment["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
            binary = work / "maze-checks"
            run([args.compiler, *flags, *mode_flags, '-I'+str(work), '-DMAZE_REFERENCE='+str(int(role == "solution")), str(root / "tests/maze-input-contract.cpp"), '-o', str(binary)])
            run([str(binary)], expected="17 malformed reloads, stream failure and 37 layouts passed\n", env=environment)
            demonstration = work / "maze-demo"
            run([args.compiler, *flags, *mode_flags, str(source), '-o', str(demonstration)])
            if role == "starter":
                expected = "".join("Layer "+str(z)+"\n"+("1 1 1 1 1 \n"*5) for z in range(5))+"Starter path length: 0\n"
            else:
                expected = "Path size: 61\nEnd coordinate: (4, 4, 4)\n"
            run([str(demonstration)], expected=expected, env=environment)
            reports.append({'role':role,'mode':mode,'malformedReloads':17,'streamFailureRejected':True,'layouts':37,'bfsReachabilityAndValidPaths':role=='solution','solverAndDriverBytesPreserved':True,'originalDemonstrationVerified':True,'unfinishedSolverPreserved':role=='starter'})
    print(json.dumps({'event':'verified-maze-input-contract','parentTaskId':task,'compiler':args.compiler,'nativeRoleModeGroups':len(reports),'roles':reports,'scope':'Exact import and reference paths; untouched learner solver remains unfinished'}), flush=True)
