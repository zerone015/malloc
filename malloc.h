#ifndef _MALLOC_H
#define _MALLOC_H

#include <stddef.h>
#include <pthread.h>
#include <errno.h>

#define chunkptr            struct t_chunk*
#define ALLOC_MAX_SIZE              
#define SIZE_SZ             (sizeof (size_t))
#define MALLOC_ALIGNMENT    16
#define MALLOC_ALIGN_MASK   (MALLOC_ALIGNMENT - 1)
#define MIN_CHUNK_SIZE      32
#define MIN_SMALL_SIZE      1024
#define MIN_LARGE_SIZE      128 * 1024

typedef struct s_state {
    pthread_mutex_t mutex;

    chunkptr  top;

    chunkptr  last_remainder;
    
    chunkptr  bins[254];
} t_state;

struct t_chunk {
  size_t  prev_size;  
  size_t  size;       

  struct t_chunk  *fd;
  struct t_chunk  *bk;

  struct t_chunk  *fd_nextsize;
  struct t_chunk  *bk_nextsize;
};

#endif