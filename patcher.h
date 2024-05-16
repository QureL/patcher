#ifndef _PATCHER_H_
#define _PATCHER_H_
#include <stdint.h>
typedef struct patcher
{
    uint8_t *src_temporary;
    int (*patch)(struct patcher *, void *);
    int (*recover)(struct patcher *);
    void *bound;
    void *src;
    long offset;
} patcher;
patcher *new_patcher();
void release_patcher(patcher *);

#endif
