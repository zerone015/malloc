#ifndef _MALLOC_H
#define _MALLOC_H

#include <stddef.h>
#include <pthread.h>

#define SIZE_SZ             (sizeof (size_t))
#define MALLOC_ALIGNMENT    16
#define MALLOC_ALIGN_MASK   (MALLOC_ALIGNMENT - 1)
#define MIN_CHUNK_SIZE      32
#define MIN_SMALL_SIZE      1024
#define MIN_LARGE_SIZE      128 * 1024
#define chunkptr            struct chunk *

struct state
{
    pthread_mutex_t mutex;

    chunkptr top;

    chunkptr last_remainder;
    
    chunkptr bins[254];
    
    unsigned int binmap[3];
};

struct chunk {

  size_t      chunk_prev_size;  
  size_t      chunk_size;       

  struct chunk* fd;         
  struct chunk* bk;

  struct chunk* fd_nextsize;
  struct chunk* bk_nextsize;
};

#endif