#include "malloc.h"

static t_state state = {
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

static size_t pad_usable_size(size_t size) {
    return size + SIZE_SZ <= MIN_CHUNK_SIZE ? MIN_CHUNK_SIZE : (size + SIZE_SZ + MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK;
}

static int out_of_range(size_t size) {
    return size >= (size_t) (-2 * MIN_CHUNK_SIZE);
}

static int is_tiny_size(size_t size) {
    return (size_t) MIN_SMALL_SIZE > size;
}

static int is_small_size(size_t size) {
    return (size_t) MIN_SMALL_SIZE <= size && (size_t) MIN_LARGE_SIZE > size;
}

static unsigned int find_tinybin_idx(size_t size) {
    return size >> 4;
}

static chunkptr find_tinychunk(size_t size) {
    chunkptr cur;

    for (unsigned int idx = find_tinybin_idx(size); idx <= MAX_TINY_INDEX; ++idx) {
        cur = state.bins[idx];
        if (cur)
        {
            state.bins[idx] = cur->fd;
            break;
        }
    }
    return cur;
}

static unsigned int find_smallbin_idx(size_t size) {
    return ((size >> 6) <= 48) ? 48 + (size >> 6) :\
           ((size >> 9) <= 20) ? 91 + (size >> 9) :\
           ((size >> 12) <= 10) ? 110 + ((size) >> 12) :\
           ((size >> 15) <= 4) ? 119 + (size >> 15) :\
           ((size >> 18) <= 2) ? 124 + (size >> 18) :\
            126;
}

static chunkptr find_smallchunk(size_t size) {
    unsigned int idx;
    chunkptr cur;
    chunkptr remainder;
    size_t remainder_size;

    idx = find_smallbin_idx(size);
    cur = state.bins[idx];
    while (size > find_chunk_size(cur)) {
        ++idx;
        if (idx > MAX_SMALL_INDEX)
            return NULL;
        cur = state.bins[idx];
    }
    cur = cur->bk;
    while (size > find_chunk_size(cur)) {
        cur = cur->fd_nextsize;
    }
    unlink(cur);
    remainder = cur + size;
    remainder_size = find_chunk_size(cur) - size;
    cur->size = size | PREV_INUSE;
    remainder->size = remainder_size | PREV_INUSE;
    categorize_chunk(remainder);
    return cur;
}

static void *find_chunk(size_t size) {
    chunkptr chunk;

    if (is_tiny_size(size)) 
        chunk = find_tinychunk(size);
    else if (is_small_size(size))
        chunk = find_smallchunk(size);
    else
        chunk = sysmalloc(size);
}

void *malloc(size_t size) {
    void *ret;

    pthread_mutex_lock(&state.mutex);
    size = pad_usable_size(size);
    if (out_of_range(size)) {
        errno = ENOMEM;
        return NULL;    
    }
    ret = find_chunk(size);
    pthread_mutex_unlock(&state.mutex);
    return ret;
}
