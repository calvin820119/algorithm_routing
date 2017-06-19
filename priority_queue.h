#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include "router.h"

void initial_heap(node_t *table);
void minimum_heapify(node_t **heap, int i);
void build_minimum_heap(void);
void heap_change_key(node_t **heap, int i);
void debug_heap(void);




#endif
