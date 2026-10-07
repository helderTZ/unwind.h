# unwind.h

Small wrapper over libbacktrace that also does demangling. Drop and use single header file. Just include it.

Required dynamic libraries: -lbracktrace, -ldl.

C++ standard: C++26 (for std::println).

## Running the example

Build the example with:

```c++
$ g++ test.cpp -std=c++26 -lbacktrace -ldl -g -o test
$ ./test
```
