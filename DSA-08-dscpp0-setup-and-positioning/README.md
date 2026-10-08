# DSCPP0: Search Work and a Repeatable Build

**Required core project.** Replace the starter's two marked search bodies, then
build and test the same small console project repeatedly. The provided driver
parses input and prints the results; the algorithm work belongs in `search.hpp`.
The three files in each role are `main.cpp`, `search.hpp` and `CMakeLists.txt`.
The reference implements the same interface and input contract. The starter
builds, but deliberately reports no matches and zero comparisons until its
TODOs are completed. Nonempty success tests should fail at that stage.

## Contract and the first trace

`SearchReport` returns an index and a comparison count. An index equal to
`values.size()` means absent, so an empty vector uses index zero as its sentinel.
Both functions leave the input unchanged and return the *first* matching index
when duplicates occur. `findFirstLinear` may inspect any vector order.
`findFirstBinary` requires nondecreasing input; the supplied driver checks that
precondition before calling it. Sorting is not hidden inside either function.

Count only comparisons of a stored value with the target. The linear function
uses one equality comparison for each visited value and stops at the first
match. For binary search, count each midpoint `< target` test and the final
candidate equality test when a candidate exists. Keep the half-open interval
`[first, last)` and compute `first + (last - first) / 2`. If the midpoint is less
than the target, exclude it and everything before it. Otherwise retain it as
an eligible first match and move `last` to it. When the interval is empty,
check the candidate before indexing. This also handles duplicates and absence.

Before running the default target 21 and values `3 8 13 21`, trace the linear
visits 3, 8, 13, 21. For binary search, trace `(first, last, middle)` as `(0,4,2)`
and `(3,4,3)`, followed by the candidate equality test. The completed output is:

```text
linear index=3 comparisons=4
binary index=3 comparisons=3
```

Change the target to 3: linear search finds it immediately, while binary search
still narrows an interval. Test a missing target below and above the range,
three duplicate matches, one value, and an empty vector. A faster result on one
small case does not establish a better worst-case complexity.

## Compile, run and test

The browser IDE edits and exports source. Extract its ZIP before compiling with
a native C++20 compiler. From either role folder:

```sh
c++ -std=c++20 -Wall -Wextra -Wpedantic -I. main.cpp -o search_work
./search_work
./search_work 4 1 4 4 4 9
./search_work 21
```

An explicit command is `./search_work TARGET [VALUE ...]`. There may be zero to
1024 values, all in nondecreasing order. Each token must be a complete integer
within the native compiler's `int` range, without a plus sign or whitespace.
Bad tokens, overflow, unsorted values and too many values produce an error on
standard error and status 2, with no search output. Absence is a normal result
with status 0. No arguments use the four-value default above.

For the same project through CMake and CTest:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

CMake compiles the program; CTest runs named input cases. After completing the
TODOs, all four supplied tests should pass. Inspect the first failing test and
its actual output, change one algorithm step, and rerun it before the full set.
For a debugger walkthrough, break inside a search loop, inspect the vector,
target, interval endpoints and count, then predict the next update before
stepping. An absent result must never become an out-of-bounds access.

## Explain the measured work

Linear search is O(n) in the worst case and O(1) when the first value matches.
Binary *search work on already sorted data* is O(log n) in the worst case. This
counter excludes input parsing, the driver's O(n) sortedness validation and
printing. The complete driver therefore still does O(n) setup work. Report
those costs separately; do not call the whole command logarithmic or infer
wall-clock speed from the counter. Neither implementation allocates another
vector. Submit the two implementations, predicted and actual results for the
changed cases, and a brief explanation of this counting boundary.

Independent study can follow the trace, marked bodies, changed inputs and
reflection in that order. In a walkthrough, pause before an interval update
and ask for its remaining candidate indices before executing it. Compare the
reference only after keeping a working attempt.

The repository's `python3 tests/verify-setup-search.py` builds both actual role
files, checks that the starter retains its marked work, and verifies reference
algorithms and CLI behavior in ordinary and sanitizer modes. Its CMake/CTest
checks cover this project only; other course units are outside that gate.
