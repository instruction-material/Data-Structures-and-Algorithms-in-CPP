# Data Structures and Algorithms in C++

This course pack is a generalized, student-facing C++ data structures and
algorithms sequence built from the same overall arc as
`Jacoba1100254352/CS235`, but rewritten for the `instruction-material`
course system.

The course assumes students have already completed an introductory C++ class.
It focuses on:

- interfaces and multi-file project structure
- graphs, recursion, sorting, and search trees
- templates and linked structures
- balancing and asymptotic reasoning
- benchmarking and engineering tradeoffs

## Projects

- `DSCPP1-Task-Manager-CLI`
- `DSCPP2-Graph-Navigation`
- `DSCPP3-Markov-Text-Generator`
- `DSCPP4-Recursive-Maze-Pathfinder`
- `DSCPP5-Quicksort-Toolkit`
- `DSCPP6-Template-Linked-List`
- `DSCPP7-Binary-Search-Tree`
- `DSCPP8-AVL-Tree`
- `DSCPP9-Performance-Benchmarks`

Every project includes:

- a `starter/` version with guided gaps or simplified behavior
- a `solution/` version with one coherent reference implementation
- a local `README.md` describing the learning goal and expected build path

## Build

```bash
cmake -S . -B build
cmake --build build
```

The root build creates a binary for every starter and solution target so the
entire pack can be smoke-tested quickly.

