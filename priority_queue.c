#include <stdlib.h>
#include "priority_queue.h"

extern FILE *fin;
extern FILE *fout;
//extern FILE *fdebug;
extern int total_routes;
extern int width;
extern int height;
extern int capacity;
extern float *horizontal_weight;
extern float *vertical_weight;
extern float *horizontal_demand;
extern float *vertical_demand;
extern float max;

extern node_t **heap;
extern int *heap_table;

static int heap_size;

/*void debug_heap(void){
	if(heap_size < 0)	return;
    int i;
    fprintf(fdebug, "\t");
    for(i=0;i<=heap_size;++i)
        fprintf(fdebug, "%4.2f(%d,%d) ", (*(heap+i))->d, (*(heap+i))->x, (*(heap+i))->y);
    if(heap_size>=0)
    fprintf(fdebug, "\t end %4.2f(%d,%d) ", (*(heap+heap_size))->d, (*(heap+heap_size))->x, (*(heap+heap_size))->y);
    fprintf(fdebug, "\n\t");
    for(i=0;i<width*height;++i)
        fprintf(fdebug, "%d ", *(heap_table+i));
    fprintf(fdebug, "\n");
}*/

void initial_heap(node_t *_table){
    int i;
    
    heap_size = (width * height)-1;
    for(i=0;i<=heap_size;++i){
        *(heap+i) = _table+i;
    }
}
void heap_change_key(node_t **heap, int i){
    int parent;
    node_t *temp;
    
    if(i == 0)    return;
    
    parent = (i-1)>>1;

    if((*(heap+i))->d < (*(heap+parent))->d){
        while((*(heap+i))->d < (*(heap+parent))->d){
            if(i==0)    break;
            
            *(heap_table+width*((*(heap+i))->y)+(*(heap+i))->x)=parent;
            *(heap_table+width*((*(heap+parent))->y)+(*(heap+parent))->x)=i;
            
            //  exchange
            temp = *(heap+i);
            *(heap+i) = *(heap+parent);
            *(heap+parent) = temp;
            
            i=(i-1)>>1;
            if(i==0)    break;
            parent=(i-1)>>1;
        }
    }else{
        minimum_heapify(heap, i);
    }
}

void minimum_heapify(node_t **heap, int i){
    int left;
    int right;
    int minimum;
    node_t *temp;
    left = (i<<1)+1;
    right = (i<<1)+2;
    
    //  find minimum
    if(left <= heap_size){
        if((*(heap+left))->d < (*(heap+i))->d){
            minimum = left;
        }else{
            minimum = i;
        }
    }else{
        minimum = i;
    }
    
    if(right <= heap_size){
        if((*(heap+right))->d < (*(heap+minimum))->d){
            minimum = right;
        }
    }
    
    if(minimum != i){
        
        *(heap_table+width*((*(heap+i))->y)+(*(heap+i))->x) = minimum;
        *(heap_table+width*((*(heap+minimum))->y)+(*(heap+minimum))->x) = i;
        
        temp = *(heap+i);
        *(heap+i) = *(heap+minimum);
        *(heap+minimum) = temp;

        minimum_heapify(heap, minimum);
    }
}

void build_minimum_heap(void){
    int i;
    for(i=(heap_size-1)/2;i>=0;--i){
        minimum_heapify(heap, i);
    }
}

int get_queue_length(node_t *table){
    return heap_size+1;
}

node_t *queue_extract_min(node_t *table){

    node_t *ret;
    ret = *(heap);
    
    *(heap) = *(heap+heap_size);
    
    *(heap_table+width*((*(heap))->y) + (*(heap))->x ) = 0;

    --heap_size;

    if(heap_size >= 0){
        minimum_heapify(heap, 0);
    }
        
    return ret;
}
