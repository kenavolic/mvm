# New Features

* add disassembly
* add example with assembler, interpreter and disassembler
* add tracing
* add profiling
* implement a high-performance backend with typical vm features (TOS caching, direct threading, meta instr, stack caching)
* tackle more advanced instruction set

# Improvements

* better unit test coverage
* Resilience to bad inputs
* better parsing/serialization of numeric values
    + efficiency
    + add support for special floating numbers (NaN, infinity)
* improve implementation of ip-modifying instructions (jump etc.)
* improve efficiency of some simple instruction handling (dup etc.)