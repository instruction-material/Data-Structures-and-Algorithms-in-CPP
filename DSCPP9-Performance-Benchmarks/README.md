# DSCPP9 Performance Benchmarks

This project closes the course by comparing data-structure behavior in code,
not just in theory.

Students practice:

- measuring insertion and lookup workloads
- comparing ordered versus hash-based behavior
- observing how custom linked and tree structures scale
- connecting timing output back to asymptotic expectations

The starter benchmarks standard containers first. Students then extend the
comparison to the custom structures they built during the course.


## Node ownership

Each custom linked or tree collection owns its allocated nodes. These teaching
types support default construction and their documented operations; copying and
assignment are disabled to avoid two destructors deleting the same nodes.
A destructor also keeps implicit move construction disabled here. Pass an
existing collection by reference. A separate deep-copy or move implementation
requires its own ownership contract and is outside this exercise.
