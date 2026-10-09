#!/usr/bin/env python3
"""Check first-match records, non-mutating views and unfinished sorting tasks."""
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
task = os.environ.get("CLASSES_FAMILY_TASK_ID", "dsa-task-record-contract")

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



flags = ["-std=c++20", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion", "-Wsign-conversion", "-Werror"]
reports = []
base_demo = "Task List\n---------\n[x] 2026-05-01 - Draft graph notes\n[ ] 2026-05-03 - Read quicksort walkthrough\n"
with tempfile.TemporaryDirectory(prefix="dsa-task-record-") as temporary:
    work = Path(temporary)
    for role in ["starter", "solution"]:
        source = root / "DSCPP1-Task-Manager-CLI" / role / "main.cpp"
        original = source.read_bytes()
        assert original.count(b"int main()") == 1
        if role == "starter":
            assert original.count(b"TODO:") == 2
            assert b"// TODO: sort by completion status and then alphabetically by description." in original
        # The tested class prefix is copied exactly. Compile the full original file
        # separately so its real demonstration is also checked without renaming main.
        (work / "task-under-test.hpp").write_bytes(original[:original.index(b"int main()")])
        for mode in ["ordinary", "sanitized"]:
            mode_flags = ["-O2"] if mode == "ordinary" else ["-O1", "-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer"]
            environment = dict(os.environ)
            environment["ASAN_OPTIONS"] = ("detect_leaks=1" if platform.system() == "Linux" else "detect_leaks=0") + ":halt_on_error=1"
            environment["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
            binary = work / "record-checks"
            run([args.compiler, *flags, *mode_flags, '-I'+str(work), '-DTASK_REFERENCE='+str(int(role == "solution")), str(root / "tests/task-record-contract.cpp"), '-o', str(binary)])
            run([str(binary)], expected="First-match records and 512 state transitions passed\n", env=environment)
            demonstration = work / "record-demo"
            run([args.compiler, *flags, *mode_flags, str(source), '-o', str(demonstration)])
            suffix = "\nTasks on 2026-05-01: 1\n" if role == "starter" else "\nTasks on 2026-05-01:\n- Draft graph notes\n"
            run([str(demonstration)], expected=base_demo+suffix, env=environment)
            reports.append({'role':role,'mode':mode,'stateTransitions':512,'classBodiesPreserved':True,'firstMatchOperations':True,'viewsDoNotMutateStorage':True,'untouchedSortingTasksPreserved':role=='starter','referenceSortingVerified':role=='solution','actualDemonstrationVerified':True})
    print(json.dumps({'event':'verified-task-record-contract','parentTaskId':task,'compiler':args.compiler,'nativeRoleModeGroups':len(reports),'roles':reports,'scope':'Record API and bounded state transitions; no interactive CLI, persistence or completed learner sorting acceptance'}), flush=True)
