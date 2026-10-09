# DSCPP1 Task Manager CLI

This project is the course on-ramp. Students practice:

- splitting a small program into types and operations
- storing records in a sequence container
- printing filtered views of data
- testing command-style behavior with a small sample set

The starter keeps the implementation intentionally simple so students can
finish sorting, filtering, and cleaner removal behavior.


## Learn the supplied record contract

The program is a fixed demonstration of command-style operations, not an
interactive command parser or a file loader. Task contains dueDate, description
and completed. There are no numeric task IDs. Dates are compared as strings;
use YYYY-MM-DD values when chronological ordering is wanted. The API does not
validate calendar dates or command syntax.

- add appends a new incomplete record. Duplicate descriptions are allowed.
- markDone updates only the first matching description in insertion order.
  A missing description leaves all records unchanged.
- remove erases only the first matching description and returns true. A missing
  description returns false without changing records. Repeating remove can erase
  another duplicate later.
- tasksOnDate returns copies of matching records. Complete the marked sorting
  task so incomplete records come first, then descriptions in alphabetical order.
- printAll prints a sorted copy by date, completion and description after the
  second marked sorting task is complete. It must not reorder stored records:
  first-match operations continue to use original insertion order.

Use a C++20 GCC/Clang compiler from either role folder:

```sh
c++ -std=c++20 -Wall -Wextra -Wpedantic -Werror main.cpp -o task-manager
./task-manager
```

The root CMake targets are dscpp1_starter and dscpp1_solution. The untouched
starter builds and keeps its two sorting tasks unfinished. Its print and filtered
views currently retain insertion order. The reference completes both views.

## Predict and implement

Trace a vector with Repeat dated 2026-05-02, another Repeat dated 2026-05-01,
and Alpha dated 2026-05-01. markDone("Repeat") changes the first record. A sorted
printed view shows the earlier date first, but remove("Repeat") still removes
the record originally inserted first. The later duplicate remains. Explain the
difference between a view's ordering and the container's ordering.

Complete the two sorting tasks. Sort a copy rather than the stored vector.
Use completion first for a single-date view, and date first for the full view;
use description as the last ordering key. Keep filtering, missing-description
behavior and first-match removal unchanged. Copying a returned Task must not
change the list. The exercise has no persistence requirement.

## Verify and explain

Test empty, one-record and multi-record lists, duplicate descriptions, missing
removal, missing completion and a date with no records. Include a full printed
view between two first-match operations to detect accidental storage reordering.
The supplied demo uses distinct descriptions, so it cannot establish duplicate
behavior by itself. Predict one duplicate trace before running it and compare
actual results with the prediction.

Run the bounded native contract gate from the repository root:

```sh
python3 tests/verify-task-record-contract.py
```

It checks actual starter/reference class bodies, 512 independent model-driven
state transitions, both original demonstrations and ordinary/sanitizer modes.
The untouched starter is checked for its unfinished insertion-order views;
that result does not grade a completed learner sorting implementation.

## Extend and review

After completing the record exercise, add an interactive command parser or file
persistence in a separate copy. Define input grammar, error messages and rejection
behavior before implementing that extension. Neither feature is supplied by this
pack. For a walkthrough, pause before the duplicate removal; for independent
study, submit the trace, sorted views and operation costs. Vector search is linear,
erasure shifts later records, and sorting a copied view takes additional storage.
