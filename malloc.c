#include "malloc.h"

static t_state state = {
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

size_t pad_usable_size(size_t size) {
    return size + SIZE_SZ <= MIN_CHUNK_SIZE ? MIN_CHUNK_SIZE : (size + SIZE_SZ + MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK;
}

int out_of_range(size_t size) {
    return size >= (size_t) (-2 * MIN_CHUNK_SIZE);
}

void *find_chunk(size_t size) {
    
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
