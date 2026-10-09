# DSCPP4 Recursive Maze Pathfinder

This project adapts the 3D maze lab into a recursion and state-management
exercise. Students practice:

- validating structured input
- mapping 3D coordinates into linear storage
- recursive backtracking
- distinguishing blocked, open, and visited state cleanly

The starter imports the maze and prints it back but intentionally leaves the
solver unfinished.


## Input and state contract

Read exactly 125 integer cells with values 0 or 1, in x-fastest order:
index = x + 5*y + 25*z for coordinates 0 through 4. Zero is blocked; one is
open. Leading and trailing whitespace are allowed. After the final cell, all
remaining input must be whitespace. Reject short input, extra values, invalid
cell values, fractional values and noninteger or overflow data. A rejected
import leaves the previously accepted maze unchanged.

The learner file keeps its solver unfinished. Implement a path from (0,0,0)
to (4,4,4) with the six axis-aligned moves, checking bounds before indexing.
The reference keeps visited state separate from the imported cells, removes
failed-branch coordinates from its current path and returns an empty path
when either endpoint is blocked or no route exists. It finds a valid path;
shortest-path length is not a requirement.

Test malformed reloads after a valid maze, not only on an empty object. Test
open, blocked, disconnected and cyclic layouts. A returned path must begin
at the entrance, end at the exit, use open cells and move exactly one unit
along one axis per step. Repeated calls must not corrupt the imported maze.

Run the ordinary and sanitizer gate from the full repository:

```sh
python3 tests/verify-maze-input-contract.py
```

A role-only IDE export contains main.cpp, not this verifier. Extract it and
use a native C++20 compiler to build and run that role. Untouched-starter
acceptance confirms input validation and the unfinished solver; it does not
grade a completed learner search implementation.
