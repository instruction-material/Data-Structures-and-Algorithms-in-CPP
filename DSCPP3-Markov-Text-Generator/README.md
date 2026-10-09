# DSCPP3 Markov Text Generator

This project turns the source course's container-heavy lab into a readable
STL sequence. Students practice:

- tokenization and cleanup
- vectors, sets, maps, and deque state windows
- simple n-gram generation
- discussing the tradeoff between randomness and determinism

The starter computes the text statistics and a basic preview but leaves the
state-based generator intentionally light.

## Learn the generation contract

The learner starter computes token statistics and prints a short preview. Its
state-based generator remains an exercise. The reference builds a map from a
window of recent tokens to the observed next tokens. Repeated successors remain
in that vector, so selection reflects their observed frequency. Initial windows
are padded with empty strings; there is no wraparound at the end of the corpus.

Window order and requested output length are nonnegative integers. Negative
values throw std::invalid_argument before constructing a state window. Order
zero is valid: it makes one memoryless distribution of all observed tokens.
An output length of zero returns no tokens. Use small orders in this classroom
exercise; memory use grows with the window length and distinct states.

## Predict and implement

For tokens alpha, beta, alpha, predict the windows and successor vectors for
orders zero, one and two. Include initial padding and the repeated alpha in the
zero-order distribution. Build transitions first, then write generation in a
separate learner copy. Keep the starter's tokenization and preview available.
The cleanup removes nonalphabetic characters and lowercases the rest; a-b becomes
ab rather than two tokens. This is a small character-based exercise, not a
Unicode tokenizer or a language model service.

## Verify and debug

Test empty and punctuation-only text, one token, repeated tokens, an order
larger than the corpus, zero order, zero output length and negative parameters.
Generation ends early when its state is absent or has no successors, so requested
length is an upper bound. Build and generate with the same window order. Check
that each generated token is an observed successor of its preceding state.

The reference starts each generation with seed 42. Repeated calls using the same
model and compiler/library produce the same sequence; the standard library's
sampling details need not produce identical sequences on different toolchains.
The native gate checks observed transitions, deterministic replay, the original
demos and ordinary/sanitizer modes:

```sh
python3 tests/verify-markov-input-contract.py
```

Use a C++20 GCC/Clang compiler for an extracted role-only IDE export. The verifier
is in the full repository, not in that export. Starter acceptance confirms its
preview and unfinished generator; it does not grade a completed learner model.

## Extend and review

Explain one transition trace, a repeated-token weight and an early-stop case.
Compare the memoryless distribution with an order-two state window. After the
required generator works, define a separate corpus-input or tokenizer extension
and its rejection behavior. Keep sample text original or public domain and run
locally. Neither network access nor file loading is supplied by this pack.
