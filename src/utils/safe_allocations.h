#ifndef __SAFE_ALLOCATIONS_H__
#define __SAFE_ALLOCATIONS_H__

#include "stdlib.h"

#ifndef CHECK_POINTER
#define CHECK_POINTER(ptr)  \
do {                    \
    if (ptr == NULL) {  \
        exit(1);        \
    }                   \
} while (0)
#endif /*CHECK_POINTER*/

/**
 * malloc(), ale ze sprawdzeniem wyniku.
 * Zakańcza działanie programu przy braku pamięci.
 *
 * @param[in] size : rozmiar wskaźnika w bajtach
 *
 * @return : zaalokowany wskaźnik
 */
static inline void *safeMalloc(size_t size)
{
    void *ptr = malloc(size);
    CHECK_POINTER(ptr);
    return ptr;
}

/**
 * realloc(), ale ze sprawdzeniem wyniku.
 * Zakańcza działanie programu przy braku pamięci.
 *
 * @param[in, out] ptr : realokowany wskaźnik
 * @param[in] size : rozmiar wskaźnika w bajtach
 *
 * @return : zrealokowany wskaźnik
 */
static inline void *safeRealloc(void *ptr, size_t size)
{
    ptr = realloc(ptr, size);
    CHECK_POINTER(ptr);
    return ptr;
}

/**
 * calloc(), ale ze sprawdzeniem wyniku.
 * Zakańcza działanie programu przy braku pamięci.
 *
 * @param[in] ptr : alokowany wskaźnik
 * @param[in] nmemb : liczba elementów pod wskaźnikiem
 * @param[in] size : rozmiar elementu w bajtach
 *
 * @return : zrealokowany wskaźnik
 */
static inline void *safeCalloc(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);
    CHECK_POINTER(ptr);
    return ptr;
}

#endif //__SAFE_ALLOCATIONS_H__
