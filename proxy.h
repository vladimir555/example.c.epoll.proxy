#ifndef HEADER_PROXY_81AC1F58_DB4B_4D4A_B70C_2E66BE87D374
#define HEADER_PROXY_81AC1F58_DB4B_4D4A_B70C_2E66BE87D374


#include <arpa/inet.h>
#include <pthread.h>


struct TProxyArguments {
    struct sockaddr_in * const server_address;
    struct sockaddr_in * const client_address;
    char               * const prefix;
};


pthread_t    startProxy(struct TProxyArguments const * const arguments);
void         stopProxy (pthread_t id);


#endif // HEADER_PROXY_81AC1F58_DB4B_4D4A_B70C_2E66BE87D374
