#include "m61.hh"
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cinttypes>
#include <cassert>
#include <sys/mman.h>
#include <map>
#include <algorithm>



// Alignment Calculator (handles alignment calculations)
class AlignmentCalculator {
public:
    static size_t max_align() {
        return alignof(std::max_align_t);
    }

    static size_t align_size(size_t size) {
        return (size + max_align() - 1) & ~(max_align() - 1);
    }

    static size_t align_position(size_t position) {
        return (position + max_align() - 1) & ~(max_align() - 1);
    }
};


// Memory Buffer Manager (manages the underlying memory)
class MemoryBuffer {
private:
    char* buffer;
    size_t pos;
    const size_t size = 8 << 20;  // 8 MiB

public:
    MemoryBuffer() : pos(0) {
        void* buf = mmap(nullptr, size, PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
        assert(buf != MAP_FAILED);
        buffer = static_cast<char*>(buf);
    }

    ~MemoryBuffer() {
        munmap(buffer, size);
    }

    // Allocate raw memory from buffer
    void* allocate(size_t sz) {
        size_t aligned_pos = AlignmentCalculator::align_position(pos);
        if (aligned_pos + sz > size) {
            return nullptr;
        }
        void* ptr = &buffer[aligned_pos];
        pos = aligned_pos + sz;
        return ptr;
    }

    size_t get_size() const { return size; }
    char* get_buffer() const { return buffer; }
};


// Statistics Manager (tracks all statistics)
class Statistics {
private:
    m61_statistics stats;

public:
    Statistics() {
        reset();
    }

    void reset() {
        stats = {0, 0, 0, 0, 0, 0, 0, 0};
    }

    void record_allocation(size_t requested_size, uintptr_t start, uintptr_t end) {
        stats.nactive++;
        stats.active_size += requested_size;
        stats.ntotal++;
        stats.total_size += requested_size;
        update_heap_bounds(start, end);
    }

    void record_free(size_t requested_size) {
        stats.nactive--;
        stats.active_size -= requested_size;
    }

    void record_failure(size_t requested_size) {
        stats.nfail++;
        stats.fail_size += requested_size;
    }

    void update_heap_bounds(uintptr_t start, uintptr_t end) {
        if (stats.heap_min == 0 || start < stats.heap_min) {
            stats.heap_min = start;
        }
        if (end > stats.heap_max) {
            stats.heap_max = end;
        }
    }

    const m61_statistics& get_statistics() const {
        return stats;
    }
};


// Block Tracker (manages active and freed blocks)
class BlockTracker {
private:
    std::map<void*, size_t> active_blocks;  // address -> requested size
    std::map<void*, size_t> freed_blocks;   // address -> aligned size

public:
    // Active blocks management
    void add_active_block(void* ptr, size_t requested_size) {
        active_blocks[ptr] = requested_size;
    }

    void remove_active_block(void* ptr) {
        active_blocks.erase(ptr);
    }

    bool is_active(void* ptr) const {
        return active_blocks.find(ptr) != active_blocks.end();
    }

    size_t get_requested_size(void* ptr) const {
        auto it = active_blocks.find(ptr);
        return (it != active_blocks.end()) ? it->second : 0;
    }

    // Freed blocks management
    void add_freed_block(void* ptr, size_t aligned_size) {
        // Start with our block as is
        void* current_ptr = ptr;
        size_t current_size = aligned_size;
        
        // Look for LEFT neighbor to merge with
        // (A neighbor that ends exactly where we start)
        auto it = freed_blocks.begin();
        while (it != freed_blocks.end()) {
            char* neighbor_end = (char*)it->first + it->second;
            if (neighbor_end == (char*)ptr) {
                // Found left neighbor! Merge it into ours
                current_ptr = it->first;
                current_size = it->second + aligned_size;
                freed_blocks.erase(it);
                break;
            }
            ++it;
        }
        
        // Look for RIGHT neighbor to merge with  
        // (A neighbor that starts exactly where we end)
        it = freed_blocks.begin();
        char* our_end = (char*)current_ptr + current_size;
        while (it != freed_blocks.end()) {
            if ((char*)it->first == our_end) {
                // Found right neighbor! Merge it into ours
                current_size += it->second;
                freed_blocks.erase(it);
                break;
            }
            ++it;
        }
        
        // Add the (possibly merged) block to freed blocks
        freed_blocks[current_ptr] = current_size;
    }

    // Find best fit from freed blocks
    void* find_best_fit(size_t aligned_sz, size_t& block_size) const {
        void* best_ptr = nullptr;
        size_t best_size = SIZE_MAX;

        for (const auto& entry : freed_blocks) {
            if (entry.second >= aligned_sz && entry.second < best_size) {
                best_ptr = entry.first;
                best_size = entry.second;
            }
        }

        block_size = best_size;
        return best_ptr;
    }

    void remove_freed_block(void* ptr) {
        freed_blocks.erase(ptr);
    }

    // Split a freed block and return leftover
    void split_freed_block(void* block_ptr, size_t block_size, size_t aligned_sz, 
                         void*& leftover_ptr, size_t& leftover_size) {
        if (block_size > aligned_sz + AlignmentCalculator::max_align()) {
            leftover_ptr = static_cast<char*>(block_ptr) + aligned_sz;
            leftover_size = block_size - aligned_sz;

            // Try to coalesce leftover with next freed block
            auto next_it = freed_blocks.upper_bound(block_ptr);
            if (next_it != freed_blocks.end() && 
                static_cast<char*>(block_ptr) + block_size == static_cast<char*>(next_it->first)) {
                leftover_size += next_it->second;
                freed_blocks.erase(next_it);
            }
        } else {
            leftover_ptr = nullptr;
            leftover_size = 0;
        }
    }

    const std::map<void*, size_t>& get_active_blocks() const {
        return active_blocks;
    }
};




// Allocator (orchestrates all components)
class MemoryAllocator {
private:
    static MemoryBuffer buffer;
    static Statistics stats_manager;
    static BlockTracker block_tracker;

public:
    static void* malloc(size_t sz, const char* file, int line) {
        (void)file; (void)line;

        // Validate input
        if (sz == 0 || sz >= SIZE_MAX) {
            stats_manager.record_failure(sz);
            return nullptr;
        }

        // Calculate alignment
        size_t aligned_sz = AlignmentCalculator::align_size(sz);

        // Try to reuse freed block
        size_t best_size;
        void* best_ptr = block_tracker.find_best_fit(aligned_sz, best_size);
        
        if (best_ptr) {
            return allocate_from_freed(best_ptr, best_size, sz, aligned_sz);
        }

        // Allocate from buffer
        return allocate_from_buffer(sz, aligned_sz);
    }

    static void free(void* ptr, const char* file, int line) {
        (void)file; (void)line;

        if (!ptr) return;

        if (block_tracker.is_active(ptr)) {
            size_t requested_size = block_tracker.get_requested_size(ptr);
            
            // Calculate aligned size for freed block
            size_t aligned_sz = AlignmentCalculator::align_size(requested_size);
            
            // Move from active to freed
            block_tracker.remove_active_block(ptr);
            block_tracker.add_freed_block(ptr, aligned_sz);
            
            // Update statistics
            stats_manager.record_free(requested_size);
        }
    }

    static void* calloc(size_t count, size_t sz, const char* file, int line) {
        if (count == 0 || sz >= SIZE_MAX / count) {
            stats_manager.record_failure(sz);
            return nullptr;
        }

        size_t total_size = count * sz;
        void* ptr = malloc(total_size, file, line);
        if (ptr) {
            memset(ptr, 0, total_size);
        }
        return ptr;
    }

    static m61_statistics get_statistics() {
        return stats_manager.get_statistics();
    }

    static void print_statistics() {
        m61_statistics stats = stats_manager.get_statistics();
        printf("alloc count: active %10llu   total %10llu   fail %10llu\n",
               stats.nactive, stats.ntotal, stats.nfail);
        printf("alloc size:  active %10llu   total %10llu   fail %10llu\n",
               stats.active_size, stats.total_size, stats.fail_size);
    }

    static void print_leak_report() {
        const auto& active_blocks = block_tracker.get_active_blocks();
        for (const auto& entry : active_blocks) {
            printf("LEAK: %p size %zu\n", entry.first, entry.second);
        }
    }

private:
    static void* allocate_from_freed(void* best_ptr, size_t best_size, 
                                    size_t requested_size, size_t aligned_sz
                                   ) {
        block_tracker.remove_freed_block(best_ptr);

        // Split if there's leftover space
        void* leftover_ptr;
        size_t leftover_size;
        block_tracker.split_freed_block(best_ptr, best_size, aligned_sz, 
                                        leftover_ptr, leftover_size);
        
        if (leftover_ptr) {
            block_tracker.add_freed_block(leftover_ptr, leftover_size);
        }

        // Track as active block
        block_tracker.add_active_block(best_ptr, requested_size);
        
        // Update statistics
        stats_manager.record_allocation(requested_size, 
                                       reinterpret_cast<uintptr_t>(best_ptr),
                                       reinterpret_cast<uintptr_t>(best_ptr) + aligned_sz - 1);

        return best_ptr;
    }

    static void* allocate_from_buffer(size_t requested_size, size_t aligned_sz
                                   ) {
        void* ptr = buffer.allocate(aligned_sz);
        if (!ptr) {
            stats_manager.record_failure(requested_size);
            return nullptr;
        }

        // Track as active block
        block_tracker.add_active_block(ptr, requested_size);
        
        // Update statistics and heap bounds
        stats_manager.record_allocation(requested_size,
                                       reinterpret_cast<uintptr_t>(ptr),
                                       reinterpret_cast<uintptr_t>(ptr) + aligned_sz - 1);

        return ptr;
    }
};


// Static instance initialization

MemoryBuffer MemoryAllocator::buffer;
Statistics MemoryAllocator::stats_manager;
BlockTracker MemoryAllocator::block_tracker;


// C interface functions

void* m61_malloc(size_t sz, const char* file, int line) {
    return MemoryAllocator::malloc(sz, file, line);
}

void m61_free(void* ptr, const char* file, int line) {
    MemoryAllocator::free(ptr, file, line);
}

void* m61_calloc(size_t count, size_t sz, const char* file, int line) {
    return MemoryAllocator::calloc(count, sz, file, line);
}

m61_statistics m61_get_statistics() {
    return MemoryAllocator::get_statistics();
}

void m61_print_statistics() {
    MemoryAllocator::print_statistics();
}

void m61_print_leak_report() {
    MemoryAllocator::print_leak_report();
}