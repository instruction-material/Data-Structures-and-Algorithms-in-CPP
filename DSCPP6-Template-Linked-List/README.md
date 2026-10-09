# DSCPP6 Template Linked List

This project brings the source linked-list lab into a modernized, student-
facing template exercise.

Students practice:

- template class design
- heap allocation and cleanup
- insert and remove logic in a singly linked list
- index-based access and string rendering for debugging

The starter includes the list structure and insert operations while leaving
students room to tighten the mutation logic.


## Node ownership

Each custom linked or tree collection owns its allocated nodes. These teaching
types support default construction and their documented operations; copying and
assignment are disabled to avoid two destructors deleting the same nodes.
A destructor also keeps implicit move construction disabled here. Pass an
existing collection by reference. A separate deep-copy or move implementation
requires its own ownership contract and is outside this exercise.
