#include <stdio.h>
#include "csapp.h"
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

/* declaration */

/* proxy */
int forward(char * packet, size_t length, struct sockaddr dest);
int parseHttpRequest(char * httpReq, size_t length, char* parseResult);
void proxy_doit(int* fd);
int parse_uri(char * uri, char * host, char * path, int * port);

int main(int argc, char* argv[])
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    if( argc != 2 ){
        printf("usage:\n"
               "     proxy [portnumber]\n");
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); 
            Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
                        port, MAXLINE, 0);
            printf("Accepted connection from (%s, %s)\n", hostname, port);
        proxy_doit(&connfd);                                           
        Close(connfd);                                            
    }
    return 0;
}

void proxy_doit(int * pfd){
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    rio_t rio;
    int fd = *pfd;

    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE)){
        return;
    }
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcmp(method, "GET")) {
        //clienterror(fd, method, "501", "Not Implemented", "Not Implemented");
        return ;
    }
    char host[MAXLINE], path[MAXLINE];
    int port;

    /* parse the request line */
    int r;
    if((r = parse_uri(uri,host,path,&port)) == -1){
        return ;
    }
    char cport[16];
    sprintf(cport, "%d", port);

    /* find if the object is in the cache */

    /* not in the cache , connect with server */
    int sockfd = open_clientfd(host,cport);
    if(sockfd == -1){
        close(sockfd);
        return;
    }
    /* request head line */
    sprintf(buf, "%s %s HTTP/1.0\r\n", method, path);
    Rio_writen(sockfd, buf, strlen(buf));
    /* generate the http request. read request from client and parse it to http 1.0 */
    while (Rio_readlineb(&rio,buf,MAXLINE) > 0){
        if(! strcmp(buf,"\r\n")) break;

        /* whether to reserve or change some headers of http */
        if(strstr(buf, "Proxy-Connection"))
            strcpy(buf, "Proxy-Connextion: close\r\n");
        else
        if(strstr(buf, "Connection"))
            strcpy(buf, "Connection: close\r\n");
        else
        if(strstr(buf, "User-Agent"))
            strcpy(buf, user_agent_hdr);

        /* send the request to the server line by line */
        printf("%s",buf);
        Rio_writen(sockfd, buf, strlen(buf));
    }
    /* send the end line */
    Rio_writen(sockfd, "\r\n", 2);

    /* recv from server */
    rio_t client_rio;
    Rio_readinitb(&client_rio, sockfd);
    char * resp = calloc(MAX_OBJECT_SIZE, sizeof(char));
    size_t n = Rio_readn(sockfd, resp, MAX_OBJECT_SIZE);
    
    /* return the object to the client */
    Rio_writen(fd, resp, n);
    /* update the cache with the new object */
}

int parse_uri(char * uri, char * host, char * path, int * port){
    char * pc;

    /* judge if uri is legal (begining with "http://") */
    if( strstr(uri,"http://") != uri ){
        fprintf(stderr, "Error: parse_uri() , illegal uri\n");
        return -1;
    }
    /* parse host , port number(if there is) */
    uri += strlen("http://");
    if((pc = strstr(uri, ":"))){
        *pc = 0;
        strcpy(host,uri);
        /* parse port number */
        *port = atoi(pc+1);
        uri  = pc + 1;
    }
    else
    if((pc = strstr(uri, "/"))){
        *pc = 0;
        strcpy(host,uri);
        uri = pc + 1;
    }
    /* parse path */
    if((pc = strstr(uri, "/"))){
        strcpy(path,pc);
    }
    else{
        path[0] = '/';
    }   
    return 0;
}