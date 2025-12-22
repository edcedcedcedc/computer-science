#include "m61.hh"
#include <cstdio>
#include <cassert>
#include <cstring>
// Check diabolical m61_calloc.

int main() {
    size_t very_large_count = (size_t) -1 / 8 + 2; // -1 is signed but converted it
    void* p = m61_calloc(very_large_count, 16);   // wraps around -> (sizet)-1 == SIZE_MAX
    assert(p == nullptr);
    m61_print_statistics();
}

//! alloc count: active          0   total          0   fail          1
//! alloc size:  active          0   total          0   fail        ???
