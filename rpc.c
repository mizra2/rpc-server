#include <stdlib.h>
#include "rpc.h"
#include "util.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

struct rpc_function {
    char *function_name;
    rpc_handler *function_handler;
};

struct rpc_server {
    int sockfd;
    int a_sockfd;
    array_t *functions;
};


// Based on Workshop 9 // Will most likely be changed later :) 
rpc_server *rpc_init_server(int port) {
    
    rpc_server *newServer = malloc(sizeof(*newServer));
    newServer->functions = createArray();

    int n;

    socklen_t client_addr_size;
    struct addrinfo hints, *res;
    int re;

    struct sockaddr_in client_addr;

    char buffer[256], ip[INET_ADDRSTRLEN];

    char service[6];

    snprintf(service, 6, "%d", port);

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int s = getaddrinfo(NULL, service, &hints, &res);

    if(s != 0) {
        printf("Error getting address info\n");
        exit(EXIT_FAILURE);
    }

    newServer->sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if(newServer->sockfd < 0) {
        printf("Error creating socket\n");
        exit(EXIT_FAILURE);
    }
    re = 1;
    if(setsockopt(newServer->sockfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(int)) < 0) {
        printf("Error reusing socket\n");
        exit(EXIT_FAILURE);
    }

    if(bind(newServer->sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        printf("Error binding address to the socket\n");
        exit(EXIT_FAILURE);
    }
    
    freeaddrinfo(res);

    if(listen(newServer->sockfd, 5) < 0) {
        printf("Error listening");
        exit(EXIT_FAILURE);
    }

    client_addr_size = sizeof client_addr;

    newServer->a_sockfd = accept(newServer->sockfd, (struct sockaddr*)&client_addr, &client_addr_size);

    if(newServer->a_sockfd < 0) {
        printf("error accepting\n");
        exit(EXIT_FAILURE);
    }

    inet_ntop(client_addr.sin_family, &client_addr.sin_addr, ip,
			  INET_ADDRSTRLEN);

    port = ntohs(client_addr.sin_port);

    printf("======================================\n\n");
    printf("New Connection From %s : %d on socket %d\n\n", ip, port, newServer->a_sockfd);
    printf("======================================\n");

    return newServer;
}

int rpc_register(rpc_server *srv, char *name, rpc_handler handler) {
    return -1;
}

void rpc_serve_all(rpc_server *srv) {

}

struct rpc_client {
    int sockfd;
};

struct rpc_handle {
    /* Add variable(s) for handle */
};


// Based on Workshop 9 // Will most likely be changed later :) 
rpc_client *rpc_init_client(char *addr, int port) {

    struct addrinfo hints, *servinfo, *rp;

    int s;
    
    rpc_client *newClient = malloc(sizeof(*newClient));

    char service[6];

    snprintf(service, 6, "%d", port);

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

    s = getaddrinfo(addr, service, &hints, &servinfo);

    if (s != 0) {
        printf("Error getting address info\n");
        exit(EXIT_FAILURE);
    }

    for(rp = servinfo; rp != NULL; rp = rp->ai_next) {
        newClient->sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if(newClient->sockfd == -1) {
            continue;
        }
        if (connect(newClient->sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            break;
        }
        close(newClient->sockfd);
    }
    if(!rp) {
        printf("Error connecting\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(servinfo);



    return newClient;
}

rpc_handle *rpc_find(rpc_client *cl, char *name) {
    return NULL;
}

rpc_data *rpc_call(rpc_client *cl, rpc_handle *h, rpc_data *payload) {
    return NULL;
}

void rpc_close_client(rpc_client *cl) {

}

void rpc_data_free(rpc_data *data) {
    if (data == NULL) {
        return;
    }
    if (data->data2 != NULL) {
        free(data->data2);
    }
    free(data);
}
