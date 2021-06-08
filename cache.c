# include "cache.h"

/* cache function */

/*
 * cte_flush - flush a cache entry
 */
void cte_flush(cte_t * ctep){
    memset(ctep,0,sizeof(cte_t));
}

/*
 * cte_match - check if a cache entry matchs a request
 */
int cte_match(cte_t * ctep, char * host, char * path){
    return !strcmp(host,ctep->host) && !strcmp(path, ctep->path) ;
}

/*
 * cache_hit - search the cache to find a hit of request
 */
cte_t * cache_hit(char * host, char * path){
    int i;
    for(i = 0; i< MAX_CACHE_LINE; i++){
        if ( cte_match(cache + i, host, path) ){
            return cache + i;
        }
    }
    return NULL;
}

/*
 * cache_put - put or update an object into cache
 */
void cache_put(cte_t * oldctep, char * host, char * path, char * content, size_t content_size){
    cte_t * dst = oldctep;
    if( !dst ){
        for(dst = cache; dst - cache < MAX_CACHE_LINE; dst++){
            if (dst->content_size == 0){
                break ;
            }
        }
        if(dst - cache == 16 ) 
            dst = cache;
    }
    cte_flush(dst);
    strcpy(dst->host, host);
    strcpy(dst->path, path);
    memcpy(dst->content, content, content_size);
    dst->content_size = content_size;
    printf("cache put: %s %s\n", host, path);
    //Write(1, dst->content, dst->content_size);
}