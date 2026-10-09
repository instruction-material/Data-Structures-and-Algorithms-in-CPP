# DSCPP7 Binary Search Tree

This project turns the source BST lab into a focused search-tree sequence.

Students practice:

- pointer-based node ownership
- recursive insertion and removal
- duplicate rejection
- tree traversal and level-order debugging output

The starter supports insertion and printing but leaves removal for the student.


## Node ownership

Each custom linked or tree collection owns its allocated nodes. These teaching
types support default construction and their documented operations; copying and
assignment are disabled to avoid two destructors deleting the same nodes.
A destructor also keeps implicit move construction disabled here. Pass an
existing collection by reference. A separate deep-copy or move implementation
requires its own ownership contract and is outside this exercise.
