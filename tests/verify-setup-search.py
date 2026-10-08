#!/usr/bin/env python3
"""Exercise the actual DSCPP0 reference, unfinished scaffold and build workflow."""
from pathlib import Path
import argparse
import json
import os
import platform
import signal
import subprocess
import tempfile
import time
import xml.etree.ElementTree as ET

parser = argparse.ArgumentParser()
parser.add_argument("--compiler", default=os.environ.get("CXX", "c++"))
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
project = root / "DSA-08-dscpp0-setup-and-positioning"
task = os.environ.get("CLASSES_FAMILY_TASK_ID", "dsa-setup-search")

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
reports = []
with tempfile.TemporaryDirectory(prefix="dsa-setup-native-") as temporary:
    work = Path(temporary)
    for mode in ["ordinary", "sanitized"]:
        mode_flags = ["-O2"] if mode == "ordinary" else [
            "-O1", "-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=all",
            "-fno-omit-frame-pointer"]
        environment = dict(os.environ)
        environment["ASAN_OPTIONS"] = ("detect_leaks=1" if platform.system() == "Linux"
                                        else "detect_leaks=0") + ":halt_on_error=1"
        environment["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
        for role in ["starter", "solution"]:
            role_dir = project / role
            sample = work / (role + "-" + mode)
            run([args.compiler, *flags, *mode_flags, str(role_dir / "main.cpp"),
                 "-o", str(sample)])
            expected = ("linear index=none comparisons=0\nbinary index=none comparisons=0\n"
                        if role == "starter" else
                        "linear index=3 comparisons=4\nbinary index=3 comparisons=3\n")
            run([str(sample)], expected=expected, env=environment)
            run([str(sample), "21"], expected="linear index=none comparisons=0\nbinary index=none comparisons=0\n", env=environment)
            invalid = [["4", "9", "1", "4"], [""], ["+4"], [" 4"], ["4 "],
                       ["4.0"], ["2147483648"], ["-2147483649"], ["4x"],
                       ["4", "1", "2x"], ["4", "1", "2147483648"],
                       ["4", *["1"] * 1025]]
            for arguments in invalid:
                run([str(sample), *arguments], expected="", env=environment, exit_code=2)
            run([str(sample), "4", *["1"] * 1024],
                expected=("linear index=none comparisons=0\nbinary index=none comparisons=0\n"
                          if role == "starter" else
                          "linear index=none comparisons=1024\nbinary index=none comparisons=10\n"),
                env=environment)
            if role == "starter":
                text = (role_dir / "search.hpp").read_text()
                assert "TODO LINEAR" in text and "TODO BINARY" in text
            else:
                check = work / ("oracle-" + mode)
                run([args.compiler, *flags, *mode_flags,
                     '-DCOURSE_HEADER="' + str(role_dir / "search.hpp") + '"',
                     str(root / "tests/setup-search.cpp"), "-o", str(check)])
                result = run([str(check)], env=environment)
                assert result.startswith("Search oracle passed: 5397 generated vector/target pairs")
                run([str(sample), "4", "1", "4", "4", "4", "9"],
                    expected="linear index=1 comparisons=2\nbinary index=1 comparisons=4\n", env=environment)
                run([str(sample), "-2147483648", "-2147483648", "0", "2147483647"],
                    expected="linear index=0 comparisons=1\nbinary index=0 comparisons=3\n", env=environment)
            reports.append({"role": role, "mode": mode, "invalidInputs": len(invalid),
                            "maximumValues": 1024, "oraclePairs": 5397 if role == "solution" else 0,
                            "unfinishedTasksPreserved": role == "starter", "passed": True})
    for role in ["starter", "solution"]:
        build = work / ("cmake-" + role)
        run(["cmake", "-S", str(project / role), "-B", str(build),
             "-DCMAKE_CXX_COMPILER=" + args.compiler])
        run(["cmake", "--build", str(build), "--parallel", "2"])
        junit = work / ("ctest-" + role + ".xml")
        run(["ctest", "--test-dir", str(build), "--output-on-failure",
             "--output-junit", str(junit)], exit_code=8 if role == "starter" else 0)
        suite = ET.parse(junit).getroot()
        assert int(suite.attrib["tests"]) == 4
        assert int(suite.attrib["failures"]) == (2 if role == "starter" else 0)
        failed = {case.attrib["name"] for case in suite.findall("testcase")
                  if case.find("failure") is not None}
        assert failed == ({"default_search", "duplicate_search"} if role == "starter" else set())
    build = work / "root-cmake"
    run(["cmake", "-S", str(root), "-B", str(build), "-DCMAKE_CXX_COMPILER=" + args.compiler])
    run(["cmake", "--build", str(build), "--target", "dscpp0_starter", "dscpp0_solution", "--parallel", "2"])
print(json.dumps({"event": "verified-setup-search", "compiler": args.compiler,
                  "reports": reports, "roleCmakeCtestVerified": True, "rootTargets": 2,
                  "scope": "DSCPP0 only; other course projects remain outside this gate"}))
