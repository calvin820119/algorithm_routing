#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "router.h"
#include "priority_queue.h"

#define weight_max 1.0f

FILE *fin;
FILE *fout;
//int total_routes;
int width;
int height;
int capacity;
float *horizontal_weight;
float *vertical_weight;
float *horizontal_demand;
float *vertical_demand;
float max;

node_t **heap;
int *heap_table;

int main(int argc, char **argv){
	
	int src_x;
	int src_y;
	int dst_x;
	int dst_y;
	int num_net;
	int i, j;
	node_t *table;
	
	char buff_20[20];
	
	if(3 != argc){
		printf("Usage: ./[exe] [input file] [output file]\n");
		exit(1);
	}
	if((FILE *)0 == (fin = fopen(*(argv+1), "r"))){
		printf(".in file access error!\n");
		exit(1);
	}
	if((FILE *)0 == (fout = fopen(*(argv+2), "w"))){
		printf(".out file access error!\n");
		exit(1);
	}

	fscanf(fin, "%s %d %d", buff_20, &width, &height);  //	fetch config
	fscanf(fin, "%s %d", buff_20, &capacity);           //	fetch config
	fscanf(fin, "%s %s %d", buff_20, buff_20, &num_net);    //	fetch config
	++capacity;
	max = (width+height)*(weight_max);
    //total_routes = 0;
	
    table = (node_t *)calloc(height*width, sizeof(node_t));
    horizontal_weight = (float *)calloc(height*(width-1), sizeof(float));
    vertical_weight = (float *)calloc(width*(height-1), sizeof(float));
    horizontal_demand = (float *)calloc(height*(width-1), sizeof(float));	//	memory allocate and clear to 0
    vertical_demand = (float *)calloc(width*(height-1), sizeof(float));  	//	memory allocate and clear to 0
    
    //  heap
    heap = (node_t **)calloc(width*height, sizeof(node_t **));
    heap_table = (int *)calloc(width*height, sizeof(int));

    for(i=0;i<height;++i){
        for(j=0;j<width;++j){
            (table+width*i+j)->x = j;
            (table+width*i+j)->y = i;
        }
    }
    
    for(i=0;i<height*(width-1);++i){
        horizontal_weight[i] = pow(2, 1.0f/capacity) - 1;
        horizontal_demand[i] = 1.0f;
	}
    for(i=0;i<width*(height-1);++i){
        vertical_weight[i] = pow(2, 1.0f/capacity) - 1;
        vertical_demand[i] = 1.0f;
	}
    
	for(i=0;i<num_net;++i){ 
		fscanf(fin, "%d %d %d %d %d", (int *)buff_20, &src_x, &src_y, &dst_x, &dst_y);
		dijkstra(table, i, table+(width*src_y)+src_x, table+(width*dst_y)+dst_x);
	}
	
	fclose(fin);
	fclose(fout);
	
	//printf("total: %d\n", (int)total_routes);
	
	return 0;
}

void initial_table(node_t *table, node_t *s){
    int x, y, i=0;
    for(y=0;y<height;++y){
    	for(x=0;x<width;++x){
	        (table+(width*y)+x)->tag = 0x0;
	        (table+(width*y)+x)->pi = (node_t *)0;
	        (table+(width*y)+x)->next = (node_t *)0;
	        (table+(width*y)+x)->d = max;
	        
	        *(heap_table+(width*y)+x) = i;
	        ++i;
	    }
	}
    s->d = 0.0f;
}

void relax(node_t *u, node_t *v, float weight){
    if(v->d > u->d + weight){
        v->d = u->d + weight;
        v->pi = u;
        heap_change_key(heap, *(heap_table+(width*v->y)+v->x));
    }
}

void dijkstra(node_t *table, int net_id, node_t *src, node_t *dst){
    int x, y, diff, step, prev_x, prev_y, res;
    node_t *u;
    
    initial_table(table, src);
    
    initial_heap(table);
    
    build_minimum_heap();
    
    while((res=get_queue_length(table))!=0){
		    
        u = queue_extract_min(table);

        y=u->y;
        x=u->x;

        u->tag = 0x1;

        //  adj.right
        if( width-1 != x )
        if( 0x0 == (table+(width*(y))+x+1)->tag){
            relax(u, table+(width*(y))+x+1, *(horizontal_weight+((width-1)*y)+x));
        }
        //  adj.left
        if( 0 != x )
        if( 0x0 == (table+(width*(y))+x-1)->tag ){
            relax(u, table+(width*(y))+x-1, *(horizontal_weight+((width-1)*y)+x-1));
        }
        //  adj.top
        if( height-1 != y )
        if( 0x0 == (table+(width*(y+1))+x)->tag ){
            relax(u, table+(width*(y+1))+x, *(vertical_weight+(width*y)+x));
        }
        //  adj.down
        if( 0 != y )
        if( 0x0 == (table+(width*(y-1))+x)->tag ){
            relax(u, table+(width*(y-1))+x, *(vertical_weight+(width*(y-1))+x));
        }
        
        if(u==dst){
            break;
        }
    }
    
    //	net_id, # of routes
    step=0;
    u = table+(width*y)+x;
    u->next = (node_t *)0;
    while((node_t *)0 != u->pi){
    	++step;
    	u->pi->next = u;
    	u = u->pi;
	}
	//total_routes += step;
	fprintf(fout, "%d %d\n", net_id, step);
	
    // footprint
    //u = table+(width*y)+x;
    u = src;
    prev_x=u->x;
    prev_y=u->y;
    u = u->next;
    
    while((node_t *)0 != u){
        
        y=u->y;
        x=u->x;
        
        //  update wv, wh
        if(prev_y - y > 0){
            //  down
            *(vertical_demand+(width*y)+x)+=1;
            if(*(vertical_demand+(width*y)+x) == capacity){
                *(vertical_weight+(width*y)+x) = max;
            }else{
                *(vertical_weight+(width*y)+x) = pow(2, *(vertical_demand+(width*y)+x)/capacity) - 1;   
            }
        }else if(prev_y - y < 0){
            //  up
            *(vertical_demand+(width*prev_y)+prev_x)+=1;
            if(*(vertical_demand+(width*prev_y)+prev_x)==capacity){
                *(vertical_weight+(width*prev_y)+prev_x) = max;
            }else{
                *(vertical_weight+(width*prev_y)+prev_x) = pow(2, *(vertical_demand+(width*prev_y)+prev_x)/capacity) - 1;    
            }
        }else if(prev_x - x > 0){
            //  left
            *(horizontal_demand+(width-1)*y+x)+=1;
            if(*(horizontal_demand+(width-1)*y+x)==capacity){
                *(horizontal_weight+(width-1)*y+x) = max;
            }else{
                *(horizontal_weight+(width-1)*y+x) = pow(2, *(horizontal_demand+(width-1)*y+x)/capacity) - 1;
            }
        }else{
            //  right
            *(horizontal_demand+((width-1)*prev_y)+prev_x)+=1;
            if(*(horizontal_demand+((width-1)*prev_y)+prev_x)==capacity){
                *(horizontal_weight+((width-1)*prev_y)+prev_x) = max;
            }else{
                *(horizontal_weight+((width-1)*prev_y)+prev_x) = pow(2, *(horizontal_demand+((width-1)*prev_y)+prev_x)/capacity) - 1;    
            }
        }
        
        fprintf(fout, "%d %d %d %d\n", prev_x, prev_y, x, y);
        u = u->next;
        prev_x = x;
        prev_y = y;
    }
}
