#ifndef __CACHE_H__
#define __CACHE_H__

#include "csapp.h"

#define MAX_CACHE_LINE 16
#define MAX_OBJECT_SIZE 102400
/* cache structure */
typedef struct {
    char host[MAXLINE];
    char path[MAXLINE];
    char content[MAX_OBJECT_SIZE];

    size_t content_size;
} cte_t;
cte_t cache[MAX_CACHE_LINE];

void cte_flush(cte_t * ctep);
int cte_match(cte_t * ctep, char * host, char * path);
cte_t * cache_hit(char * host, char * path);
void cache_put(cte_t * oldctep, char * host, char * path, char * content, size_t content_size);

#endif