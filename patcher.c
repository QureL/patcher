#include "patcher.h"
#include <stdint.h>
#include <sys/errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define PATCHER_CMD_LENGTH 12
#define ERR_PRINT(buff)                                   \
    sprintf(buff, "mprotect:%d err=%d", __LINE__, errno); \
    perror(buff);
#if defined(__i386__) || defined(__x86_64__)
static uint8_t *build_patch_cmd(uint64_t addr)
{
    uint8_t *res = (uint8_t *)malloc(PATCHER_CMD_LENGTH);
    if (!res)
    {
        return NULL;
    }
    uint8_t tmp[sizeof(addr)] = {0};
    for (int i = 0; i < sizeof(addr); i++)
    {
        tmp[i] = addr;
        addr >>= 8;
    }
    res[0] = 0x48;
    res[1] = 0xBA;
    res[PATCHER_CMD_LENGTH - 2] = 0xFF;
    res[PATCHER_CMD_LENGTH - 1] = 0xE2;
    memcpy(res + 2, tmp, sizeof(addr));
    return res;
}
#endif

#ifdef __linux__
#include <unistd.h>
#include <sys/mman.h>

static void *get_page_bound(void *ptr)
{
    long page_size = sysconf(_SC_PAGESIZE);
    return (void *)((long)ptr & ~(page_size - 1));
}

static long get_distance_to_bound(void *ptr)
{
    long page_size = sysconf(_SC_PAGESIZE);
    return (long)ptr & (page_size - 1);
}

int patch(patcher *p, void *target)
{
    if (!p)
        return -1;
    char buff[128] = {0};
    uint8_t *cmd = NULL;

    cmd = build_patch_cmd((uint64_t)target);
    if (!cmd)
        return -1;

    p->bound = get_page_bound(p->src);
    p->offset = get_distance_to_bound(p->src);

    if (mprotect(p->bound, PATCHER_CMD_LENGTH + p->offset, PROT_WRITE | PROT_READ | PROT_EXEC) == -1)
    {
        ERR_PRINT(buff);
        return -1;
    }

    memcpy(p->src_temporary, p->src, PATCHER_CMD_LENGTH);
    memcpy(p->src, cmd, PATCHER_CMD_LENGTH);
    if (mprotect(p->bound, PATCHER_CMD_LENGTH + p->offset, PROT_READ | PROT_EXEC) == -1)
    {
        ERR_PRINT(buff);
        free(cmd);
        return -1;
    }
    free(cmd);
    return 0;
}
int recover(patcher *p)
{
    if (!p)
        return -1;
    char buff[128] = {0};
    if (mprotect(p->bound, PATCHER_CMD_LENGTH + p->offset, PROT_WRITE | PROT_READ | PROT_EXEC) == -1)
    {
        ERR_PRINT(buff);
        return -1;
    }
    memcpy(p->src, p->src_temporary, PATCHER_CMD_LENGTH);
    if (mprotect(p->bound, PATCHER_CMD_LENGTH + p->offset, PROT_READ | PROT_EXEC) == -1)
    {
        ERR_PRINT(buff);
        return -1;
    }
}

#endif

patcher *new_patcher(void *src)
{
    patcher *p = (patcher *)malloc(sizeof(patcher));
    if (!p)
        return NULL;

    p->src_temporary = (uint8_t *)malloc(PATCHER_CMD_LENGTH);
    if (!p->src_temporary)
    {
        free(p);
        return NULL;
    }
    p->patch = patch;
    p->recover = recover;
    p->src = src;
    return p;
}

void release_patcher(patcher *p)
{
    if (!p)
        return;
    free(p->src_temporary);
    free(p);
}