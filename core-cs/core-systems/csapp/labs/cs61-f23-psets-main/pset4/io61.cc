#include "io61.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <climits>
#include <cerrno>

// io61.cc
//    YOUR CODE HERE!



/*
io61_file
Data structure for io61 file wrappers. Add your own stuff.

FD	meaning
0	stdin
1	stdout
2	stderr
3	your opened file (blockcat61.cc)
*/
struct io61_file {
    int fd = -1;     // file descriptor
    int mode;        // open mode (O_RDONLY or O_WRONLY)
    io61_fcache* cache;
};

/*
io61_fcache
-----------
Single-slot file cache for buffered I/O.

Fields:
- fd       : File descriptor associated with this cache.
- bufsize  : Fixed cache capacity (4096 bytes = 4 KiB).
- cbuf     : Memory buffer that stores cached file data.
- tag      : File offset of the first valid byte in the cache.
- pos_tag  : Current logical file position; the next byte to read or write.
- end_tag  : File offset one past the last valid byte in the cache.

Cache Layout:
- The cache represents the half-open interval [tag, end_tag).
- Valid bytes are stored in:
      cbuf[0 .. end_tag - tag - 1]
- The current position within the cache is:
      cbuf[pos_tag - tag]

Core Invariants:
- tag <= pos_tag <= end_tag
- end_tag - tag <= bufsize

Derived Quantities:
- end_tag - tag
    = number of valid bytes currently stored in the cache
    = cache_size()

- pos_tag - tag
    = number of bytes already processed
    = current buffer index
    = cache_bytes_processed()

- end_tag - pos_tag
    = number of unread bytes remaining in a read cache
    = cache_bytes_remaining()

Special States:
- tag == pos_tag == end_tag
    => cache is empty (contains no valid data)

- end_tag - tag == bufsize
    => cache is full (all buffer slots contain valid data)

Membership Rule:
- tag <= off < end_tag
    => file offset `off` is currently cached

Buffer Mapping Rule:
- cbuf[off - tag]
    => buffer location corresponding to file offset `off`

Cache Operations:
- cache_reset()
    Empties the cache while preserving the current logical file position.
    After reset:
        tag = pos_tag = end_tag

- cache_fill()
    Reads up to `bufsize` bytes from the file descriptor into `cbuf`,
    starting at the current kernel file position (which should equal `tag`).
    On success:
        end_tag = tag + bytes_read

Read Cache Interpretation:
- [tag, pos_tag)
    = bytes already consumed

- [pos_tag, end_tag)
    = bytes available to read without another system call

- [end_tag, tag + bufsize)
    = unused buffer space

Write Cache Invariant:
- pos_tag == end_tag
    (used when buffering writes)

Slot-Fit Condition:
- sz <= bufsize
- pos_tag + sz <= tag + bufsize

This guarantees that `sz` bytes starting at `pos_tag`
fit entirely within the current cache slot.
*/
struct io61_fcache {
    int fd;
    static constexpr off_t bufsize = 4096;
    unsigned char cbuf[bufsize];
    
    off_t tag;
    off_t pos_tag;
    off_t end_tag;
   
    off_t bytes_remaining()
    {
        return end_tag - pos_tag;
    }
    void reset()
    {
        tag = pos_tag = end_tag = 0;
    }
    bool is_empty()
    {
        return pos_tag >= end_tag;
    }
    bool is_full()
    {
        return end_tag - tag == bufsize;
    }
    void check_invariants()
    {
        assert(tag <= pos_tag);
        assert(pos_tag <= end_tag);
        assert(end_tag - tag <= bufsize);
    }
    void check_write_invariant()
    {
        assert(pos_tag == end_tag);
    }
};

static ssize_t io61_fill(io61_fcache* c){
    c->check_invariants();
    c->tag = c->pos_tag; 

    ssize_t n = read(c->fd, c->cbuf, c->bufsize);
    
    if (n < 0)
    {
        return -1;
    }
        
    c->end_tag = c->tag + n;
    c->check_invariants();

    return n;
}

// io61_fdopen(fd, mode)
//    Returns a new io61_file for file descriptor `fd`. `mode` is either
//    O_RDONLY for a read-only file or O_WRONLY for a write-only file.
//    You need not support read/write files.
io61_file* io61_fdopen(int fd, int mode) {
    assert(fd >= 0);

    io61_file* f = new io61_file;

    f->fd = fd;
    f->mode = mode;

    f->cache = new io61_fcache;

    f->cache->fd = fd;
    f->cache->reset();
    return f;
}


// io61_close(f)
//    Closes the io61_file `f` and releases all its resources.
int io61_close(io61_file* f) {
    int r = io61_flush(f);
    int r2 = close(f->fd);
    delete f->cache;
    delete f;
    if(r < 0) return r;
    return r2;
}


// io61_readc(f)
//    Reads a single (unsigned) byte from `f` and returns it. Returns EOF,
//    which equals -1, on end of file or error.
int io61_readc(io61_file* f) {
    unsigned char ch;
    ssize_t nr = io61_read(f, &ch, 1);
    if (nr == 1) {
        return ch;
    } else if (nr == 0) {
        errno = 0; // clear `errno` to indicate EOF
        return -1;
    } else {
        assert(nr == -1 && errno > 0);
        return -1;
    }
}


// io61_read(f, buf, sz)
//    Reads up to `sz` bytes from `f` into `buf`. Returns the number of
//    bytes read on success. Returns 0 if end-of-file is encountered before
//    any bytes are read, and -1 if an error is encountered before any
//    bytes are read.
//
//    Note that the return value might be positive, but less than `sz`,
//    if end-of-file or error is encountered before all `sz` bytes are read.



//    READ direction: file → cbuf → buf - cache is source of truth for reads
ssize_t io61_read(io61_file* f, unsigned char* buf, size_t sz) {
    io61_fcache* c = f->cache;
    c->check_invariants();

    size_t total = 0;
    while(total < sz)
    {
        if(c->is_empty())
        {
            ssize_t nr = io61_fill(c);
            if(nr < 0)
            {
                return -1;
            }
            else if(nr == 0)
            {
                break;
            }
            
        }

    size_t avail = c->bytes_remaining();
    size_t chunk = sz - total;
    if(chunk > avail)
    {
        chunk = avail;
    }
    memcpy(buf + total, c->cbuf + (c->pos_tag - c->tag), chunk);
    c->pos_tag += chunk;
    total += chunk;

    }
    return total;  
}


// io61_writec(f)
//    Write a single character `c` to `f` (converted to unsigned char).
//    Returns 0 on success and -1 on error.
int io61_writec(io61_file* f, int c) {
    unsigned char ch = c;
    ssize_t nw = io61_write(f, &ch, 1);
    if (nw == 1) 
    {
        return 0;
    } 
    else 
    {
        return -1;
    }
}


// io61_write(f, buf, sz)
//    Writes `sz` characters from `buf` to `f`. Returns `sz` on success.
//    Can write fewer than `sz` characters when there is an error, such as
//    a drive running out of space. In this case io61_write returns the
//    number of characters written, or -1 if no characters were written
//    before the error occurred.
ssize_t io61_write(io61_file* f, const unsigned char* buf, size_t sz) {
    io61_fcache* c = f->cache;
    c->check_invariants();
    c->check_write_invariant();
    
    size_t total = 0;
    while (total < sz) 
    {   
        size_t buffered = c->end_tag - c->tag;
        size_t space = c->bufsize - buffered;
        
        if(space == 0)
        {
            if (io61_flush(f) < 0) 
            {
                if(total == 0)
                {
                    return -1;
                }
                else
                {
                    return total;
                }
                
            }
           buffered = 0;
           space = c->bufsize;
        }

       size_t chunk = sz - total;
       if (chunk > space)
       {
            chunk = space;
       }
       memcpy(c->cbuf + buffered, buf + total, chunk);
       c->end_tag+=chunk;
       c->pos_tag = c->end_tag; //maintain write invariant
       total+=chunk;
        
    }
    return total;
}


// io61_flush(f)
//    If `f` was opened write-only, `io61_flush(f)` forces a write of any
//    cached data written to `f`. Returns 0 on success; returns -1 if an error
//    is encountered before all cached data was written.
//
//    If `f` was opened read-only, `io61_flush(f)` returns 0. It may also
//    drop any data cached for reading.
int io61_flush(io61_file* f) {
    io61_fcache* c = f->cache;
    c->check_invariants();

    if (f->mode == O_RDONLY)
    {
        c->reset();
        return 0;
    }
    
    size_t nbytes = c->end_tag - c->tag;
    if (nbytes == 0)
    {
        return 0;
    }
    unsigned char* p = c->cbuf;
    off_t pos = c->tag;
    while(nbytes > 0)
    {
        ssize_t nw = write(f->fd, c->cbuf, nbytes);
        if(nw < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        nbytes -=nw;
        p+=nw;
        pos+=nw;
    }
    c->tag = c->pos_tag = c->end_tag;
    return 0;
}



// io61_seek(f, off)
//    Changes the file pointer for file `f` to `off` bytes into the file.
//    Returns 0 on success and -1 on failure.
int io61_seek(io61_file* f, off_t off) {
    if (f->mode == O_WRONLY && io61_flush(f) < 0)
    {
        return -1;
    } 

    //invalidate cache and move logical position
    f->cache->tag = f->cache->pos_tag = f->cache->end_tag = off;
    if (lseek(f->fd, off, SEEK_SET) == -1)
    {
        return -1;
    }
    return 0;
}

// You shouldn't need to change these functions.

// io61_open_check(filename, mode)
//    Opens the file corresponding to `filename` and returns its io61_file.
//    If `!filename`, returns either the standard input or the
//    standard output, depending on `mode`. Exits with an error message if
//    `filename != nullptr` and the named file cannot be opened.
io61_file* io61_open_check(const char* filename, int mode) {
    int fd;
    if (filename) {
        fd = open(filename, mode, 0666);
    } else if ((mode & O_ACCMODE) == O_RDONLY) {
        fd = STDIN_FILENO;
    } else {
        fd = STDOUT_FILENO;
    }
    if (fd < 0) {
        fprintf(stderr, "%s: %s\n", filename, strerror(errno));
        exit(1);
    }
    return io61_fdopen(fd, mode & O_ACCMODE);
}


// io61_fileno(f)
//    Returns the file descriptor associated with `f`.
int io61_fileno(io61_file* f) {
    return f->fd;
}


// io61_filesize(f)
//    Returns the size of `f` in bytes. Returns -1 if `f` does not have a
//    well-defined size (for instance, if it is a pipe).

off_t io61_filesize(io61_file* f) {
    struct stat s;
    int r = fstat(f->fd, &s);
    if (r >= 0 && S_ISREG(s.st_mode)) {
        return s.st_size;
    } else {
        return -1;
    }
}
