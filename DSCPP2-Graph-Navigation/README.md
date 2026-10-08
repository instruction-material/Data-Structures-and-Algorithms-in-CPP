# DSCPP2 Graph Navigation

Find a least-cost route through a directed weighted graph. Practice adjacency
matrices, Dijkstra selection, parent reconstruction and validation before mutation.

## Roles and build

The starter supplies a working O(V^2) baseline, input validation, path printing
and cost checking. Its marked task is to replace linear minimum selection with
a priority queue while preserving the contract. This is an algorithm comparison,
so compiling or reproducing the sample alone does not finish the assignment.
The reference uses a min-priority queue and ignores stale queued distances.

From either role folder, build with a C++20 GCC/Clang compiler:

```sh
c++ -std=c++20 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror main.cpp -o graph
./graph
```

The supplied driver reads a fixed five-node fixture. The starter prints
`Starter path: 0 2 3 4`; the reference prints `Shortest path: 0 2 3 4` and
`Cost: 7`. Both use the same API. The driver is a small demonstration, not an
interactive file loader. Use a separate tester with an istringstream to change
input without rewriting the algorithm. The root CMake targets are
`dscpp2_starter` and `dscpp2_solution`.

## Input and failure contract

Use an ordinary non-throwing input stream. The first integer is a node count
from 1 through 256, followed by exactly V*V integer edge weights. Nodes are
numbered 0 through V-1. A weight is -1 for an absent directed edge, or any
nonnegative int through INT_MAX. Zero-weight edges and cycles are allowed.
Rows need not be symmetric; diagonal entries may be -1 or a valid weight.
Only whitespace may follow the matrix. A negative weight other than -1,
missing weight, numeric overflow, oversized count, trailing token or bad
stream rejects the candidate. Rejection returns false and preserves the last
accepted graph. A successful reload replaces it completely. Allocation failure
can still throw; the existing graph remains intact because publication is a swap.

Matrix storage is O(V^2). Costs and queued distances use signed 64-bit values;
every simple path in a supported graph fits, including edges above the former
int sentinel. Returning a path does not change graph state. An invalid endpoint
or unreachable goal returns an empty path. A node reaches itself with one node
and cost zero. Equal-cost paths need not have the same node sequence across the
two implementations. Compare reachability and total cost, not one tie ordering.

costOfPath checks every node and consecutive edge, including a single-node path.
It throws invalid_argument for an unknown node or absent edge, and overflow_error
if an arbitrary long walk would exceed the cost type. An empty path has cost zero;
check whether shortestPath returned a route before interpreting a cost as reachability.

## Predict, change and explain

Before running, trace why 0 -> 2 -> 3 -> 4 costs 7 and beats the route through
node 1. Keep a table of settled distances, parents and the next selection.
Change an edge so the optimal path changes, then test a directed unreachable
goal, a zero-cost cycle and start equal to goal. Test two INT_MAX edges in a
three-node chain: the valid result is 4,294,967,294, not unreachable.

Load a valid graph, then attempt a matrix whose last row is incomplete. Verify
the original path and cost remain available. Repeat with -2 and an extra token.
Explain why validating a candidate before swapping differs from clearing state
after a late error.

For the marked task, use queue entries containing cost and node. Initialize the
start to zero, relax only present edges, update a parent only on a strict cost
improvement, and skip stale entries. Reconstruct after determining reachability.
With an adjacency matrix, a queue still scans V potential neighbors per settled
node; it does not give the sparse adjacency-list complexity automatically.
Measure or count selections separately from file parsing and output.

For a walkthrough, predict one changed route and pause before one relaxation.
For independent study, write the trace and failure-state explanation before
testing. Submit both implementations' results, one rejected reload and the reason
for using a wider cost type. The regression checks compare bounded cases with an
independent all-pairs oracle; they do not grade an unseen explanation.

Run the graph regression gate from the repository root:

```sh
python3 tests/verify-graph-navigation.py
```

It builds both actual role files with warnings as errors, tests changed and
malformed inputs, and runs ordinary and address/undefined-behavior sanitizer
modes. Other course projects are outside this graph-specific gate.
