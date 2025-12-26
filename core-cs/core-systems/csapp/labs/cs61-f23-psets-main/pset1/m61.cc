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
    size_t buffer_size;

public:
    MemoryBuffer() : buffer_size(8 << 20) {
        void* buf = mmap(nullptr, buffer_size, PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
        assert(buf != MAP_FAILED);
        buffer = static_cast<char*>(buf);
    }

    ~MemoryBuffer() {
        munmap(buffer, buffer_size);
    }

    size_t get_size() const { return buffer_size; }
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

    bool is_in_heap(void* ptr) const {
        uintptr_t addr = (uintptr_t)ptr;

        if (stats.heap_min == 0 && stats.heap_max == 0) {
            return false;
        }
        //heap_max is inclusive (the last byte of the heap)
        return addr >= stats.heap_min && addr <= stats.heap_max;
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

    bool is_freed(void* ptr) const {
        return freed_blocks.find(ptr) != freed_blocks.end();
    }

    size_t get_requested_size(void* ptr) const {
        auto it = active_blocks.find(ptr);
        return (it != active_blocks.end()) ? it->second : 0;
    }

    // Freed blocks management
    void add_freed_block(void* ptr, size_t aligned_size, char* buffer_start, size_t* buffer_pos_ptr) {
        // Simple approach: always try to merge with wilderness first
        char* block_end = (char*)ptr + aligned_size;
        char* wilderness_start = buffer_start + *buffer_pos_ptr;
        
        if (block_end == wilderness_start) {
            // Block is adjacent to wilderness - merge by moving wilderness back
            *buffer_pos_ptr = (char*)ptr - buffer_start;
            
            // Now check if any freed blocks are now adjacent to the new wilderness
            // We need to keep merging until no more merges are possible
            bool merged;
            do {
                merged = false;
                for (auto it = freed_blocks.begin(); it != freed_blocks.end(); ++it) {
                    char* freed_block_end = (char*)it->first + it->second;
                    if (freed_block_end == buffer_start + *buffer_pos_ptr) {
                        // Found adjacent freed block - merge it too
                        *buffer_pos_ptr = (char*)it->first - buffer_start;
                        freed_blocks.erase(it);
                        merged = true;
                        break;
                    }
                }
            } while (merged);
        } else {
            // Block is not adjacent to wilderness - add to freed blocks
            // Use the existing merging logic
            void* current_ptr = ptr;
            size_t current_size = aligned_size;
            
            // Look for LEFT neighbor to merge with
            auto it = freed_blocks.begin();
            while (it != freed_blocks.end()) {
                char* neighbor_end = (char*)it->first + it->second;
                if (neighbor_end == (char*)ptr) {
                    current_ptr = it->first;
                    current_size = it->second + aligned_size;
                    freed_blocks.erase(it);
                    break;
                }
                ++it;
            }
            
            // Look for RIGHT neighbor to merge with  
            it = freed_blocks.begin();
            char* our_end = (char*)current_ptr + current_size;
            while (it != freed_blocks.end()) {
                if ((char*)it->first == our_end) {
                    current_size += it->second;
                    freed_blocks.erase(it);
                    break;
                }
                ++it;
            }
            
            freed_blocks[current_ptr] = current_size;
        }
    }

    // Find best fit from freed blocks or wilderness
    void* find_best_fit(size_t aligned_sz, size_t& block_size, 
                       char* buffer_start, size_t buffer_pos, size_t buffer_size) const {
        void* best_ptr = nullptr;
        size_t best_size = SIZE_MAX;

        // Check freed blocks
        for (const auto& entry : freed_blocks) {
            if (entry.second >= aligned_sz && entry.second < best_size) {
                best_ptr = entry.first;
                best_size = entry.second;
            }
        }
        
        // Also check wilderness (never-allocated space at end)
        size_t wilderness_start = buffer_pos;
        size_t wilderness_size = buffer_size - wilderness_start;
        if (wilderness_size >= aligned_sz && wilderness_size < best_size) {
            best_ptr = buffer_start + wilderness_start;
            best_size = wilderness_size;
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

    bool is_within_active_block(void* ptr) const {
        for (const auto& entry : active_blocks) {
            void* block_start = entry.first;
            size_t requested_size = entry.second;
            size_t aligned_size = AlignmentCalculator::align_size(requested_size);
            
            if (ptr >= block_start && 
                static_cast<char*>(ptr) < static_cast<char*>(block_start) + aligned_size) {
                return true;
            }
        }
        return false;
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
    static size_t buffer_pos;  // Track current allocation position

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

        // Try to reuse freed block or wilderness
        size_t best_size;
        void* best_ptr = block_tracker.find_best_fit(aligned_sz, best_size,
                                                    buffer.get_buffer(),
                                                    buffer_pos,
                                                    buffer.get_size());
        
        if (best_ptr) {
            // Check if it's from wilderness
            char* wilderness_start = buffer.get_buffer() + buffer_pos;
            if (best_ptr == (void*)wilderness_start) {
                // Allocate from wilderness
                buffer_pos += aligned_sz;
                
                // Track as active block
                block_tracker.add_active_block(best_ptr, sz);
                
                // Update statistics
                stats_manager.record_allocation(sz,
                    reinterpret_cast<uintptr_t>(best_ptr),
                    reinterpret_cast<uintptr_t>(best_ptr) + aligned_sz - 1);
                
                return best_ptr;
            } else {
                // Allocate from freed block
                return allocate_from_freed(best_ptr, best_size, sz, aligned_sz);
            }
        }

        // Allocate from buffer (wilderness)
        return allocate_from_buffer(sz, aligned_sz);
    }

    static void free(void* ptr, const char* file, int line) {
        if (!ptr) return;
        
        if (!stats_manager.is_in_heap(ptr)) {
            // Updated: Added file and line to error message for test 42
            fprintf(stderr, "MEMORY BUG: %s:%d: invalid free of pointer %p, not in heap\n", 
                    file, line, ptr);
            return;
        }

        if (block_tracker.is_active(ptr)) {
            size_t requested_size = block_tracker.get_requested_size(ptr);
            
            // Calculate aligned size for freed block
            size_t aligned_sz = AlignmentCalculator::align_size(requested_size);
            
            // Move from active to freed
            block_tracker.remove_active_block(ptr);

            // Pass buffer info for wilderness merging
            block_tracker.add_freed_block(ptr, aligned_sz, 
                                         buffer.get_buffer(), 
                                         &buffer_pos);
            
            // Update statistics
            stats_manager.record_free(requested_size);
        } else {
            if (block_tracker.is_within_active_block(ptr)) {
                // Updated: Added file and line to error message for test 42
                fprintf(stderr, "MEMORY BUG: %s:%d: invalid free of pointer %p, not allocated\n", 
                        file, line, ptr);
            } else {
                // Updated: Added file and line to error message for test 42
                fprintf(stderr, "MEMORY BUG: %s:%d: invalid free of pointer %p, double free\n", 
                        file, line, ptr);
            }
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
                                    size_t requested_size, size_t aligned_sz) {
        block_tracker.remove_freed_block(best_ptr);

        // Split if there's leftover space
        void* leftover_ptr;
        size_t leftover_size;
        block_tracker.split_freed_block(best_ptr, best_size, aligned_sz, 
                                        leftover_ptr, leftover_size);
        
        if (leftover_ptr) {
            block_tracker.add_freed_block(leftover_ptr, leftover_size, 
                                         buffer.get_buffer(), &buffer_pos);
        }

        // Track as active block
        block_tracker.add_active_block(best_ptr, requested_size);
        
        // Update statistics
        stats_manager.record_allocation(requested_size, 
                                       reinterpret_cast<uintptr_t>(best_ptr),
                                       reinterpret_cast<uintptr_t>(best_ptr) + aligned_sz - 1);

        return best_ptr;
    }

    static void* allocate_from_buffer(size_t requested_size, size_t aligned_sz) {
        size_t aligned_pos = AlignmentCalculator::align_position(buffer_pos);
        if (aligned_pos + aligned_sz > buffer.get_size()) {
            stats_manager.record_failure(requested_size);
            return nullptr;
        }
        void* ptr = &buffer.get_buffer()[aligned_pos];
        buffer_pos = aligned_pos + aligned_sz;
        
        // Track as active block
        block_tracker.add_active_block(ptr, requested_size);
        
        // Update statistics
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
size_t MemoryAllocator::buffer_pos = 0;

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