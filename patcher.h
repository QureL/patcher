#ifndef _PATCHER_H_
#define _PATCHER_H_

void *get_page_bound(void *ptr);
long get_offset_to_bound(void *ptr);
int exchange_addr(void *src, void *target);

#endif
