/* Header for RPC system */
/* Please do not modify this file */

#ifndef RPC_H
#define RPC_H

#include <stddef.h>


/* Server state */
typedef struct rpc_server rpc_server;
/* Client state */
typedef struct rpc_client rpc_client;

typedef struct rpc_function rpc_function;

typedef struct thread_content thread_content;


/* The payload for requests/responses */
typedef struct {
    int data1; // Max Size Of Size (64 Bits)
    size_t data2_len; // What it means to of size_t
    void *data2;
} rpc_data;

/* Handle for remote function */
typedef struct rpc_handle rpc_handle;

/* Handler for remote functions, which takes rpc_data* as input and produces
 * rpc_data* as output */
typedef rpc_data *(*rpc_handler)(rpc_data *);

/* ---------------- */
/* Server functions */
/* ---------------- */

/* Initialises server state */
/* RETURNS: rpc_server* on success, NULL on error */
rpc_server *rpc_init_server(int port);

/* Registers a function (mapping from name to handler) */
/* RETURNS: -1 on failure */
int rpc_register(rpc_server *srv, char *name, rpc_handler handler);

/* Start serving requests */
void rpc_serve_all(rpc_server *srv);

/* ---------------- */
/* Client functions */
/* ---------------- */

/* Initialises client state */
/* RETURNS: rpc_client* on success, NULL on error */
rpc_client *rpc_init_client(char *addr, int port);

/* Finds a remote function by name */
/* RETURNS: rpc_handle* on success, NULL on error */
/* rpc_handle* will be freed with a single call to free(3) */
rpc_handle *rpc_find(rpc_client *cl, char *name);

/* Calls remote function using handle */
/* RETURNS: rpc_data* on success, NULL on error */
rpc_data *rpc_call(rpc_client *cl, rpc_handle *h, rpc_data *payload);

/* Cleans up client state and closes client */
void rpc_close_client(rpc_client *cl);

/* ---------------- */
/* Shared functions */
/* ---------------- */

/* Frees a rpc_data struct */
void rpc_data_free(rpc_data *data);

void test_function_execution(rpc_server *test);

void test_call_function(rpc_client *cl, rpc_handle *h);

void *test_multithreading(void * s);


#endif
