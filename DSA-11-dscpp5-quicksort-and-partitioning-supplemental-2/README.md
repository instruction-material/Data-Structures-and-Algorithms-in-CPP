# Quicksort Partition Transfer Worksheet

Continue the saved **DSCPP5 Quicksort Toolkit** core project. Use its README
and the same inclusive-range pivot/partition contract. This is optional practice,
not another required starter program.

1. Partition `[4, 1, 4, -2, 4]` around the value at index 2. Predict the pivot's
   returned index and the two regions before executing the code. Equal values
   belong on the greater-or-equal side; the regions need not be sorted.
2. Partition only indices 1 through 4 of `[99, 4, 1, 4, -2, 88]`, with pivot
   index 3. Explain why 99 and 88 must remain untouched.
3. Verify empty input through `sortAll`, then one- and two-item inputs. State the
   supplied tiny-range pivot rule rather than inventing a third sample entry.
4. Use a separately sorted copy and value counts to check final sorting, then
   inspect partition boundaries independently. Record any failed expectation.

The historical `starter/main.cpp` and `solution/main.cpp` in this folder are
retained for earlier links and saved imports. Their vector transformation is
unrelated to quicksort and is not the assignment for this worksheet. Continue
or export the saved core project instead of replacing it with these files.
