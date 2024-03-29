/*   /\_/\  
    ( o.o )  ♥ ~ < Milad Izra Project 2 Computer Systems 1263635 >
     > ^ <   */ 

#include <stdlib.h>
#include "rpc.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#define NONBLOCKING

#define RUNNING 1

#define CONNECTION_ESTABLISHED 1

#define MAX_NAME_LENGTH 1000

#define FIND "find"

#define CALL "call"

#define END "end!"




//     ╱|、
//   (˚ˎ 。7  
//    |、˜〵   ♥ ~ < Data Strucutres >      
//   じしˍ,)ノ

struct rpc_function {
    char function_name[MAX_NAME_LENGTH + 1];
    rpc_handler function_handler;
};

struct rpc_server {
    // Server Socket ID
    int sockfd;
    // Accepting Socket ID 
    int a_sockfd;
    // Array List Of Functions
    array_t *functions;
    pthread_mutex_t socket_mutex;
};

struct rpc_handle {

    uint32_t n;
    
};

// Based on Workshop 9 // 
rpc_server *rpc_init_server(int port) {
    
    // Allocate Memory 
    rpc_server *newServer = malloc(sizeof(*newServer));
    newServer->functions = createArray();

    struct addrinfo hints, *res;

    // Convert To String

    char service[6];

    snprintf(service, 6, "%d", port);

    memset(&hints, 0, sizeof hints);

    // IPV6
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int s = getaddrinfo(NULL, service, &hints, &res);

    if(s != 0) {
        printf("Error getting address info\n");
        exit(EXIT_FAILURE);
    }
    struct addrinfo *p;

    // From Lecture Slides 
    
    // Since we are using IPV6 have to go through possible socket

    for(p = res; p != NULL; p = p -> ai_next) {
        if(p->ai_family == AF_INET6 && (newServer->sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
            printf("Error creating socket\n");
            exit(EXIT_FAILURE);
        }
    }


    // Per Spec Sheet Request
    int enable = 1;
    if (setsockopt(newServer->sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if(bind(newServer->sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        printf("Error binding address to the socket\n");
        exit(EXIT_FAILURE);
    }
    
    freeaddrinfo(res);


    return newServer;
}


// Register a function to the server
int rpc_register(rpc_server *srv, char *name, rpc_handler handler) {
    // If one of these empty then return -1 per spec sheet
    if(!srv || !name || !handler) {
        // Failed
        return -1;
    }

    // Create a new function and allocate memory for it

    rpc_function *newFunction = malloc(sizeof(*newFunction));
    newFunction->function_handler = handler;

    strcpy(newFunction->function_name, name);

    // Add function to the server

    // Check if the function already exists 

    for(int i = 0; i < srv->functions->n; i++) {
        if(!strcmp(srv->functions->F[i]->function_name, name)) {
            // Replace it if it does
            srv->functions->F[i] = newFunction;
            return 1;
        }
    } 

    appendArray(srv->functions, newFunction);

    // Print registered funciton to ensure that it is being added correctly (Testing)
    // printf("%d: Registered Function: %s\n", srv->functions->n, name);

    // Success if return 1 (Per Spec Sheet)
    return 1;
}

void rpc_serve_all(rpc_server * srv) {

    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
    client_addr_size = sizeof client_addr;

    if(listen(srv->sockfd, 5) < 0) {
        printf("Error listening");
        exit(EXIT_FAILURE);
    }

    while (RUNNING) {
        // Accept a new connection
        srv->a_sockfd = accept(srv -> sockfd, (struct sockaddr * ) & client_addr, & client_addr_size);


        if (srv->a_sockfd < 0) {
            // No Connection Found, Keep Listening For Incoming Requests
            continue;
        } else {
            // If a connection is found, handle it's requests by creating a new thread
            pthread_t thread_id;
            if(pthread_create(&thread_id, NULL, client_handler, (void*) srv) != 0) {
                printf("Failed to create thread!\n");
            }
        }

    }
}


struct rpc_client {
    int sockfd;
};

// Based on Workshop 9 
rpc_client *rpc_init_client(char *addr, int port) {

    struct addrinfo hints, *servinfo, *rp;

    int s;
    
    rpc_client *newClient = malloc(sizeof(*newClient));

    // Assume port will max 6 digits.

    char service[6];

    snprintf(service, 6, "%d", port);

    memset(&hints, 0, sizeof hints);

    // IPv6

    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

    s = getaddrinfo(addr, service, &hints, &servinfo);

    if (s != 0) {
        printf("Error getting address info\n");
        free(newClient);
        return NULL;
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


// Returns a structure that contains a int32 that represents an index for the array contained on the server
rpc_handle *rpc_find(rpc_client *cl, char *name) {

    int n;

    rpc_handle *handler = malloc(sizeof(*handler));

    assert(handler);

    handler->n = -1;
    
    // Send a "find" request to the server

    n = write(cl->sockfd, FIND, 5);

    if (n < 0) {

		perror("write");
		exit(EXIT_FAILURE);

	}

    // Write function name to server

    n = write(cl->sockfd, name, strlen(name));

    if(n < 0) {
        perror("write");
		exit(EXIT_FAILURE);
    }

    // Get back the index of the function on server

    n = read(cl->sockfd, &handler->n, sizeof(uint32_t));

    if(n < 0) {
        perror("read");
		exit(EXIT_FAILURE);
    }

    handler->n = ntohl(handler->n);

    if(handler->n == -1) {
        return NULL;
    }


    return handler;
}

// Calls a function on the server
rpc_data *rpc_call(rpc_client *cl, rpc_handle *h, rpc_data *payload) {

    // Handle bad data

    if(cl == NULL || h == NULL || payload == NULL) {
        return NULL;
    }

    if(payload->data2_len == 0 && payload->data2 != NULL) {
        perror("Bad data");
        return NULL;
    }
    if(payload->data2_len > 0 && payload->data2 == NULL) {
        perror("Bad Data!");
        return NULL;
    }
    if(payload->data2_len < 0 || payload->data2_len > 100000) {
        perror("Bad Data!");
        return NULL;
    }

    int n;

    rpc_data *newData = malloc(sizeof(*newData));

    // Write to server we are calling a function

    n = write(cl->sockfd, CALL, 5);

    if (n < 0) {

		perror("write");
		exit(EXIT_FAILURE);
        
	}

    // Convert to network byte

    h->n = htonl(h->n);

    n = write(cl->sockfd, &h->n, sizeof(uint32_t));


    // Sending Data To The Server (Client -> Server)
    // ======================== // 
    // Data 1 

    // Send payload data 1 & convert it to a networtk byte 

    int64_t value = htobe64((int64_t)payload->data1);

    n = write(cl->sockfd, &value, sizeof(int64_t));


    // Data 2 Len (Client -> Server)

    // Convert size_t of type uint32_t so we can convert it to a network byte and send it over

    uint32_t convertedValue = (uint32_t)payload->data2_len;

    convertedValue = htonl(convertedValue);

    n = write(cl->sockfd, &convertedValue, sizeof(uint32_t));

    // Data 2 Field (Client -> Server)

    // Send data 2 field

    if(payload->data2_len != 0) {
        void *data2 = payload->data2;

        n = write(cl->sockfd, data2, payload->data2_len);
    } 

    // End Of Sending Data To Server



    // Check Function Execution Status
    uint32_t status;
    n = read(cl->sockfd, &status, sizeof(uint32_t));
    status = ntohl(status);
    if(status == 0) {
        fprintf(stderr, "ERROR: Failed to execute function call\n");
        return NULL;
    } else if (status == 1) {
        goto new_data_read;
    }

new_data_read:

    // Receive Data Back From Server (Server -> Client)
    // ======================== // 
    
    // Same thing but in reverse

    // Data 1 (Server -> Client)
    int64_t value_d;
    n = read(cl->sockfd, &value_d, sizeof(int64_t));
    newData->data1 = (int)be64toh(value_d);


    // Data 2 Len (Server -> Client)

    uint32_t data2_l; 

    n = read(cl->sockfd, &data2_l, sizeof(uint32_t));

    newData->data2_len = (size_t)ntohl(data2_l);

    // Data 2 Field (Server -> Client)

    if(newData->data2_len != 0) {
        void *data2 = malloc(newData->data2_len);
        assert(data2);

        n = read(cl->sockfd, data2, newData->data2_len);

        newData->data2 = data2;
    } else {
        newData->data2 = NULL;
    }

    // End Of Receive Data Back From Server

    return newData;
}

void rpc_close_client(rpc_client *cl) {

    int n;

    if (!cl) return;
    if(cl->sockfd != -1) {

        // Connection is still open 
        // Send a request to close connection;

        n = write(cl->sockfd, END, 5);

        if (n < 0) {

            perror("read");
            exit(EXIT_FAILURE);
        
	    }

        // Read signal closed. 

        uint32_t signal_closed;

        n = read(cl->sockfd, &signal_closed, sizeof(uint32_t));
        signal_closed = htonl(signal_closed);

        if(signal_closed == 1) {
            // Close Connection
            close(cl->sockfd);
            cl->sockfd = -1;
        }
    }

    free(cl);
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


//        (\ /)
//       ( . .) ♥ ~< Testing Functions  For "add2" & Mutlithreading =) >
//       c(")(")

void *client_handler(void * s) {

    rpc_server *srv = (rpc_server*)s;

    int a_sockfd = srv->a_sockfd;

    char buffer[256];

    int n;

    while (CONNECTION_ESTABLISHED) {

        // leave space for null term
        n = read(a_sockfd, buffer, 5);
        if (n < 0) {
            printf("Error reading\n");
            exit(EXIT_FAILURE);
        }

        buffer[n] = '\0';

        // Handle Close Client Request

        if (!strcmp(buffer, "end!")) {

            uint32_t close_signal = 1;

            close_signal = ntohl(close_signal);

            n = write(a_sockfd, & close_signal, sizeof(uint32_t));

            close(a_sockfd);
            break;
        }

        // Handle Find Call Request

        if (!strcmp(buffer, FIND)) {

            n = read(a_sockfd, buffer, 255);

            if (n < 0) {
                printf("Error reading");
                exit(EXIT_FAILURE);
            }
            buffer[n] = '\0';

            // printf("%s\n", buffer);
            // printf("%s\n", functionName);
            // Loop thru possible functions
            int found = -1;
            for (int i = 0; i < srv -> functions -> n; i++) {

                // printf("Functions: %d %s\n", i, srv -> functions -> F[i] -> function_name);

                if (!strcmp(buffer, srv -> functions -> F[i] -> function_name)) {

                    uint32_t value = htonl(i);

                    n = write(a_sockfd, & value, sizeof(uint32_t));

                    found = 1;
                    break;

                }
            }

            // Handle failed find by sending -1

            if (found == -1) {
                uint32_t value = htonl(-1);

                n = write(a_sockfd, & value, sizeof(uint32_t));
            }
        }

        // Handle a function call request by a client

        if (!strcmp(buffer, CALL)) {

            // Accept Data Regarding Index of Function
            uint32_t value;

            n = read(a_sockfd, & value, sizeof(uint32_t));

            value = ntohl(value);

            // Per Protocal 
            // Receive Data From Client (Client -> Server)
            // ======================== // 


            rpc_data * data = malloc(sizeof( * data));
            assert(data);

            // Data 1 (Client -> Server)
            int64_t test;
            n = read(a_sockfd, & test, sizeof(int64_t));
            data -> data1 = (int) be64toh(test);

            // Data 2 Len (Client -> Server)

            uint32_t data2_len;

            n = read(a_sockfd, & data2_len, sizeof(uint32_t));

            data -> data2_len = (size_t) ntohl(data2_len);

            // Data 2 Field (Client -> Server)

            if (data -> data2_len != 0) {
                void * data2 = malloc(data -> data2_len);
                assert(data2);

                n = read(a_sockfd, data2, data -> data2_len);
                data -> data2 = data2;
            } else {
                data -> data2 = NULL;
            }

            // End Reciving Data From Client
            // ======================== // 

            // Function Call 

            rpc_data *newData = srv -> functions -> F[value] -> function_handler(data);


            if(newData == NULL) {
                perror("Bad data");
            }
            else if(newData->data2_len == 0 && newData->data2 != NULL) {
                perror("Bad data");
                // send something to client here?
                rpc_data_free(newData);
                newData = NULL;
            }
            else if(newData->data2_len > 0 && newData->data2 == NULL) {
                perror("Bad Data!");
                rpc_data_free(newData);
                newData = NULL;
            }
            else if(newData->data2_len < 0 || newData->data2_len > 100000) {
                perror("Bad Data!");
                rpc_data_free(newData);
                newData = NULL;
            }
            

            if (!newData) {
                // SEND FAILED CALL SIGNAL 
                uint32_t status = 0;
                status = htonl(status);
                n = write(a_sockfd, & status, sizeof(uint32_t));
                
            } else {
                uint32_t status = 1;
                status = htonl(status);
                n = write(a_sockfd, & status, sizeof(uint32_t));

             

                // Send New Data Back To Client If Successful
                // ======================== // 

                // Data 1 (Server -> Client)

                int64_t value_d = htobe64((int64_t) newData -> data1);
                n = write(a_sockfd, & value_d, sizeof(int64_t));

           

                // Data 2 Len (Server -> Client)

                uint32_t convertedValue = (uint32_t) newData -> data2_len;

                convertedValue = htonl(convertedValue);

                n = write(a_sockfd, & convertedValue, sizeof(uint32_t));

                

                // Data 2 Field (Server -> Client)
                if (newData -> data2_len != 0) {
                    void * data2 = newData -> data2;

                    n = write(a_sockfd, data2, newData -> data2_len);


                }

            }

            // Free data
            rpc_data_free(data);
            rpc_data_free(newData);
        }
    }
    return NULL;
}


// Array Util Functions From Project 1 
// Create an empty array
array_t *createArray() {
    array_t *arr = malloc(sizeof(*arr));
    assert(arr);
    int size = 2;
    arr->size = size;
    arr->F = malloc(size * sizeof(*(arr->F)));
    assert(arr->F);
    arr->n = 0;
    return arr;
}

// Ensure an array has enough room
void ensureArraySize(array_t *arr) {
    if(arr->n == arr->size) {
        arr->size *= 2;
        arr->F = realloc(arr->F, arr->size * sizeof(*(arr->F)));
        assert(arr->F);
    }
}

// Add function to an array
void appendArray(array_t *arr, rpc_function *n) {
    ensureArraySize(arr);
    arr->F[arr->n] = n;
    (arr->n)++;
}

// Free the array
void arrayFree(array_t *arr) {
	for (int i = 0; i < arr->n; i++) {
		free(arr->F[i]);
	}
	free(arr->F);
	free(arr);
}