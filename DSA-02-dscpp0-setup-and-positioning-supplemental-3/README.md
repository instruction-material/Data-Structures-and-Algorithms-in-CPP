# Build and Debug Transfer Worksheet

**Optional worksheet; use an exported copy of the core project.** Reading this
worksheet imports no program. Keep the original saved attempt first.

Build the copy with warnings enabled and run CTest. Temporarily change one
completed search return value so `default_search` fails; record its expected
and actual output, restore the implementation, and show the test passing.
Separately try `./search_work 4 9 1 4` and explain why input rejection has status
2 but an absent target in a sorted list has status 0. Use a debugger breakpoint
at one binary interval update and record `(first, last, middle)` before and
after stepping. Explain the discarded candidate positions.

Submit the failing/passing test evidence, the two exit statuses and the trace.
The intentional fault belongs only in the copy. This worksheet checks the
build/test/debug workflow; it is not another required search implementation.

The legacy `starter` and `solution` subfolders remain available for older
catalog links and saved imports. Their numeric-transform exercise is not this
worksheet or a new DSA assignment. Use the completed setup search project above.
