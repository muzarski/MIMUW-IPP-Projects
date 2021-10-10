#include <stdio.h>
#include <stdlib.h>

// Both of these functions exit program with code 1
// if there is no heap memory available.
void* safe_malloc (size_t size) {

    void *p = malloc (size);

    if (size > 0 && p == NULL)
        exit(EXIT_FAILURE);

    return p;

}
void* safe_realloc (void *ptr, size_t size) {

    void *p = realloc (ptr, size);

    if(size > 0 && p == NULL)
        exit(EXIT_FAILURE);

    return p;

}