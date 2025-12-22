#include "m61.hh"
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cinttypes>
#include <cassert>
#include <sys/mman.h>
#include <iostream>
#include <map>

struct m61_memory_buffer {
    char* buffer;
    size_t pos = 0;
    size_t size = 8 << 20; /* 8 MiB */

    m61_memory_buffer();
    ~m61_memory_buffer();
};

static m61_memory_buffer default_buffer;
static m61_statistics default_stats = {
    .nactive = 0,
    .active_size = 0,
    .ntotal = 0,
    .total_size = 0,
    .nfail = 0,
    .fail_size = 0,
    .heap_min = 0,
    .heap_max = 0
};
std::map<void*, size_t> freed_set;
std::map<void*, size_t> active_sizes;

m61_memory_buffer::m61_memory_buffer() {
    void* buf = mmap(nullptr,    // Place the buffer at a random address
        this->size,              // Buffer should be 8 MiB big
        PROT_WRITE,              // We want to read and write the buffer
        MAP_ANON | MAP_PRIVATE, -1, 0);
                                 // We want memory freshly allocated by the OS
    assert(buf != MAP_FAILED);
    this->buffer = (char*) buf;
}

m61_memory_buffer::~m61_memory_buffer() {
    munmap(this->buffer, this->size);
}




/// m61_malloc(sz, file, line)
///    Returns a pointer to `sz` bytes of freshly-allocated dynamic memory.
///    The memory is not initialized. If `sz == 0`, then m61_malloc may
///    return either `nullptr` or a pointer to a unique allocation.
///    The allocation request was made at source code location `file`:`line`.
void* m61_malloc(size_t sz, const char* file, int line) {
    (void) file; (void) line;

    if (sz == 0 || sz >= SIZE_MAX) {
        default_stats.nfail++;
        default_stats.fail_size += sz;
        return nullptr;
    }

    // Try to reuse a freed block
    // Its not aligning because it was already aligned 
    // from when it was first allocated
    // the original allocation took care of it 
    for (auto it = freed_set.begin(); it != freed_set.end(); ++it) 
    {
        if (it->second >= sz) 
        {   
            //byte-wise arithmetic
            char* ptr = static_cast<char*>(it->first);  
            size_t block_size = it->second;

            freed_set.erase(it);

            // Split the block if there's leftover
            if (block_size > sz) 
            {
                char* remaining_ptr = ptr + sz;
                freed_set[remaining_ptr] = block_size - sz;
            }

            // Track ONLY what was allocated
            active_sizes[ptr] = sz;

            // Stats (use sz, not block_size)
            default_stats.nactive++;
            default_stats.active_size += sz;
            default_stats.ntotal++;
            default_stats.total_size += sz;

            return ptr;
        }
    }

    // Allocate from default_buffer
    size_t alignment = alignof(std::max_align_t);
    size_t aligned_pos = (default_buffer.pos + alignment - 1) & ~(alignment - 1);

    if (aligned_pos + sz > default_buffer.size) 
    {
        // Not enough space
        default_stats.nfail++;
        default_stats.fail_size += sz;
        return nullptr;
    }

    void* ptr = &default_buffer.buffer[aligned_pos];
    default_buffer.pos = aligned_pos + sz;

    // Update stats
    active_sizes[ptr] = sz;
    default_stats.nactive++;
    default_stats.active_size += sz;
    default_stats.ntotal++;
    default_stats.total_size += sz;
    // Update heap
    unsigned long long start = (unsigned long long)ptr;
    unsigned long long end = start + sz - 1;
    if (default_stats.heap_min == 0 || start < default_stats.heap_min) default_stats.heap_min = start;
    if (end > default_stats.heap_max) default_stats.heap_max = end;

    return ptr;
}


/// m61_free(ptr, file, line)
///    Frees the memory allocation pointed to by `ptr`. If `ptr == nullptr`,
///    does nothing. Otherwise, `ptr` must point to a currently active
///    allocation returned by `m61_malloc`. The free was called at location
///    `file`:`line`.

void m61_free(void* ptr, const char* file, int line) {
    (void) file; (void) line; // suppress warnings
    if (!ptr) return;

    auto it = active_sizes.find(ptr);
    if (it != active_sizes.end()) 
    {
        size_t sz = it->second;
        active_sizes.erase(it);
        freed_set[ptr] = sz;

        default_stats.nactive--;
        default_stats.active_size -= sz;
    }
}


/// m61_calloc(count, sz, file, line)
///    Returns a pointer a fresh dynamic memory allocation big enough to
///    hold an array of `count` elements of `sz` bytes each. Returned
///    memory is initialized to zero. The allocation request was at
///    location `file`:`line`. Returns `nullptr` if out of memory; may
///    also return `nullptr` if `count == 0` or `size == 0`.

void* m61_calloc(size_t count, size_t sz, const char* file, int line) {
    // Your code here (not needed for first tests).
    if(count == 0 || sz >= SIZE_MAX / count)
    {   
        default_stats.nfail++;
        default_stats.fail_size += sz;
        return nullptr;
    }
    void* ptr = m61_malloc(count * sz, file, line);
    if (ptr) {
        memset(ptr, 0, count * sz);
    }
    return ptr;
}

/// m61_get_statistics()
///    Return the current memory statistics.

m61_statistics m61_get_statistics() {
    // Your code here.
    // The handout code sets all statistics to enormous numbers.
    //m61_statistics stats;
    //memset(&stats, 0, sizeof(m61_statistics));
    return default_stats;
}

/// m61_print_statistics()
///    Prints the current memory statistics.

void m61_print_statistics() {
    m61_statistics stats = m61_get_statistics();
    printf("alloc count: active %10llu   total %10llu   fail %10llu\n",
           stats.nactive, stats.ntotal, stats.nfail);
    printf("alloc size:  active %10llu   total %10llu   fail %10llu\n",
           stats.active_size, stats.total_size, stats.fail_size);
}


/// m61_print_leak_report()
///    Prints a report of all currently-active allocated blocks of dynamic
///    memory.

void m61_print_leak_report() {
    // Your code here.
}
