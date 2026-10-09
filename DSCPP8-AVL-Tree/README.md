# DSCPP8 AVL Tree

This project follows directly from the BST unit and adds balancing.

Students practice:

- maintaining node heights
- identifying left-left, left-right, right-right, and right-left cases
- rotations as structural repairs
- preserving BST removal rules while restoring balance

The starter intentionally behaves like a plain BST so students can add the AVL
logic themselves.


## Node ownership

Each custom linked or tree collection owns its allocated nodes. These teaching
types support default construction and their documented operations; copying and
assignment are disabled to avoid two destructors deleting the same nodes.
A destructor also keeps implicit move construction disabled here. Pass an
existing collection by reference. A separate deep-copy or move implementation
requires its own ownership contract and is outside this exercise.
