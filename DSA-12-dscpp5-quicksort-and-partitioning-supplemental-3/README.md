# Quicksort Partition Extension Worksheet

Continue the saved **DSCPP5 Quicksort Toolkit** core project. Complete and
verify its required three tasks before choosing this optional extension.

Choose one investigation and state the changed contract:

- Count partition comparisons for sorted, reversed, all-equal and fixed-seed
  random inputs of small equal sizes. Copy the same input for each run and
  independently check every result. Explain why a pivot rule alone does not
  guarantee balanced partitions; counts are not measured execution times.
- Recurse into the smaller partition and loop over the larger one. Preserve
  inclusive bounds, value counts and the original partition properties. Explain
  how this bounds active recursion depth while quadratic time can remain.
- Implement a three-way partition with separate less-than, equal-to and
  greater-than regions. Define the two returned boundaries and recurse only
  outside the equal region. Verify all-equal and mixed-duplicate cases before
  comparing its operation counts with the original two-way scheme.

Present the original and changed behavior with independently expected results.
Keep experiments bounded and separate generation, copying and correctness
checks from any optional timing. No single timing run establishes a general
performance claim.

The historical `starter/main.cpp` and `solution/main.cpp` in this folder are
retained for earlier links and saved imports. Their vector transformation is
unrelated to quicksort and is not the assignment for this worksheet. Continue
or export the saved core project instead of replacing it with these files.
