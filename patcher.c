#include "patcher.h"
#include <stdint.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#define PATCHER_CMD_LENGTH 12
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

void *get_page_bound(void *ptr)
{
    long page_size = sysconf(_SC_PAGESIZE);
    return (void *)((long)ptr & ~(page_size - 1));
}

long get_distance_to_bound(void *ptr)
{
    long page_size = sysconf(_SC_PAGESIZE);
    return (long)ptr & (page_size - 1);
}

int exchange_addr(void *src, void *target)
{
    void *bound = get_page_bound(src);
    long offset = get_distance_to_bound(src);
    char buff[128] = {0};
    if (mprotect(bound, PATCHER_CMD_LENGTH + offset, PROT_WRITE | PROT_READ | PROT_EXEC) == -1)
    {
        sprintf(buff, "mprotect:%d err=%d", __LINE__, errno);
        perror(buff);
        return -1;
    }
    uint8_t *cmd = build_patch_cmd(target);
    if (!cmd)
    {
        return -1;
    }
    memcpy(src, cmd, PATCHER_CMD_LENGTH);
    if (mprotect(bound, PATCHER_CMD_LENGTH + offset, PROT_READ | PROT_EXEC) == -1)
    {
        sprintf(buff, "mprotect:%d err=%d", __LINE__, errno);
        perror(buff);
        free(cmd);
        return -1;
    }
    free(cmd);
    return 0;
}

#endif