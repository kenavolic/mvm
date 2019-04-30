# New Features

* enable comments in assembly
* add profiling
* yaml to .h generator

# Improvements

* add early and comprehensive compile time failures
* add different level of tracing
* introduce typical vm preformance features ? (TOS caching, direct threading, meta instr, stack caching)
* improve efficiency of some simple instruction handling (dup, rot n) without pop all and push all behavior
* better unit test coverage
* resilience to bad inputs
* better parsing/serialization of numeric values:
    + efficiency
    + add support for special floating numbers (NaN, infinity)