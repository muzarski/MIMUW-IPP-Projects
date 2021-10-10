#ifndef SAFE_ALLOCS_H
#define SAFE_ALLOCS_H

// initial capacity for malloc/realloc functions
#define INITIAL_CAPACITY 4

// Functions that safely allocate heap memory
void* safe_malloc (size_t size);
void* safe_realloc (void *ptr, size_t size);

#endif // SAFE_ALLOCS_H