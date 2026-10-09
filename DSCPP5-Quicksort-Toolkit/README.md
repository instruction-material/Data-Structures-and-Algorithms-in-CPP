# DSCPP5 Quicksort Toolkit

Required project: implement median-of-three pivot selection, partitioning and
recursive quicksort over the provided `std::vector<int>`. The vector owns and
grows the storage; this pack does not implement a raw buffer or manual memory
management. Each role is one standalone `main.cpp` program. Start with
`starter/main.cpp`; `solution/main.cpp` is a complete comparison reference.

## Build, run and observe the unfinished starter

From the chosen role folder:

```sh
c++ -std=c++20 -Wall -Wextra -Wpedantic main.cpp -o quicksort
./quicksort
```

The untouched starter reports:

```text
Starter values (quicksort pending): 7, 2, 9, 4, 1, 8
```

The supplied `add`, `toString`, demonstration and `sortAll` entry point are
complete. The three marked tasks are `medianOfThree`, `partition` and
`quickSort`. The starter deliberately leaves the input unsorted. It does not
call `std::sort` to pass the demonstration before quicksort is implemented.
After those tasks are complete, the starter's same label prints
`1, 2, 4, 7, 8, 9`. The reference reports
`Sorted values: 1, 2, 4, 7, 8, 9`.

## Interval and pivot contract

All algorithm ranges are **inclusive**: `[left, right]`. Public pivot/partition
calls require `0 <= left <= right < item count`, and `pivotIndex` must be inside
that range. The demonstration uses small classroom inputs, with indices and
`left + right` representable by `int`. Invalid public indices are outside this
pack's contract. An empty vector is handled by `sortAll` before forming a range.

For at least three items, `medianOfThree` orders the sampled left, middle and
right entries and returns the middle index, where `middle = (left + right) / 2`.
It changes those entries, not the whole range. A one-item range returns that
index; for two items, the reference orders the pair and returns the left index.
The tiny-range rule avoids pretending there are three distinct sample entries.

`partition` copies the pivot value, moves its entry to `right`, and scans
`[left, right)`. Before each scan step, `[left, storeIndex)` contains values
strictly less than the pivot, `[storeIndex, i)` contains values greater than or
equal to it, and `[i, right)` is still unexamined. Swap a smaller value into
`storeIndex`, then advance that boundary. Finally move the pivot from `right`
into `storeIndex` and return its new position. Entries outside the requested
range and the multiset inside it must remain unchanged.

For `[7, 2, 9, 4, 1, 8]`, the samples are 7, 9 and 8. Ordering the samples gives
`[7, 2, 8, 4, 1, 9]`, so the pivot is 8 at index 2. Moving it to the end gives
`[7, 2, 9, 4, 1, 8]`. Scanning and the final swap give
`[7, 2, 4, 1, 8, 9]`, with returned pivot index 4. The left region contains
values below 8; the right region contains values at least 8. Neither region
has to be sorted yet.

## Recursion and correctness

`quickSort` returns immediately when `left >= right`. Otherwise it obtains a
pivot index, partitions, then sorts `[left, newPivot - 1]` and
`[newPivot + 1, right]`. The pivot is excluded from both calls, and each range
shrinks. For the trace above, those calls use `[0, 3]` and `[5, 5]`.

Duplicates belong on the greater-or-equal side in this partition scheme. All
items equal is therefore an important worst case: one side may lose only the
pivot on every call. Median-of-three does not guarantee balanced partitions.
Expected balanced work is O(n log n); worst-case time is O(n²), and this direct
recursive version can use O(n) call-stack depth. It is not stable. Keep stress
experiments bounded; stack-depth reduction or three-way partitioning is an
optional extension with its own stated contract.

## Independent verification and walkthrough

Before running code, write the expected sequence with all original values and
duplicate counts. Compare results with a separately sorted copy using
`std::sort` only in a test driver, outside the submitted algorithm. Check empty,
singleton, two-item, duplicate-heavy, negative/extreme integer, sorted,
reverse-sorted and fixed-seed random inputs. Repeated `sortAll` calls must retain
the sorted result; adding an item after a sort must work on the next call.

Check pivot selection and partition separately, including interior subranges.
Record the pivot value before partitioning; afterward verify the returned index,
strictly smaller left values, greater-or-equal right values, unchanged outside
entries and equal value counts. Final sorted output alone cannot establish
these intermediate properties.

A walkthrough can follow the same sequence: predict the worked partition,
trace its scan boundary, complete pivot selection, complete partition, complete
recursive bounds, and compare independent expected results. Saved core work
continues through each step. The optional transfer and extension worksheets
use this same project; they do not require importing an unrelated program.

Repository maintainers can run the bounded native acceptance harness from the
repository root:

```sh
python3 tests/verify-quicksort-contract.py --compiler c++
```
