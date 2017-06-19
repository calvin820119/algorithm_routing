#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <stdio.h>

typedef struct node_s{
    int x;
    int y;
    float d;
    int tag;
    struct node_s *pi;
    struct node_s *next;
}node_t;

void dijkstra(node_t *table, int net_id, node_t *src, node_t *dst);
node_t *queue_extract_min(node_t *table);
int get_queue_length(node_t *table);
void relax(node_t *u, node_t *v, float weight);
void initial_table(node_t *table, node_t *s);

#endif
