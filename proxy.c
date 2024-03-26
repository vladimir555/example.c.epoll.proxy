#include "proxy.h"

#define __STDC_WANT_LIB_EXT1__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <stdatomic.h>

#include "logger.h"


static int const        MAX_EVENTS      = 16;
static int const        BUFFER_SIZE     = 128;
static int const        EPOLL_TIMEOUT   = 1000;

static atomic_bool      is_running;
static int              is_initialized  = 0;
static pthread_mutex_t  mutex           = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t   condition       = PTHREAD_COND_INITIALIZER;


int connectTCPClient(struct sockaddr_in const * const address, int epoll_fd) {
    int   fd    = 0;
    char *error = NULL;

    logger_log("client socket connection ...");

    do {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            error = "client socket creation failed";
            break;
        }

        int result = 0;

        result = connect(fd,
            (struct sockaddr * const)
                    address,
            sizeof(*address));

        if (result < 0) {
            error = "client socket connection failed";
            break;
        }

        result = fcntl(fd, F_GETFL, 0);
        if (result < 0) {
            error = "client socket fcntl get flags failed";
            break;
        }

        result = fcntl(fd, F_SETFL, result | O_NONBLOCK);
        if (result < 0) {
            error = "client socket fcntl set flags failed";
            break;
        }

        struct epoll_event epoll_event_subscription = {
            .events     = EPOLLHUP | EPOLLERR,
            .data.fd    = fd
        };

        result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &epoll_event_subscription);

        if (result < 0) {
            error = "epoll_ctl: add client socket failed";
            break;
        }

    } while(0);

    if (error) {
        logger_log("client socket connection error: %s", error);
        close(fd);
        return 0;
    } else {
        logger_log("client socket connection OK");
        return fd;
    }
}


void *runProxyThread(void * const arguments_) {
    pthread_mutex_lock(&mutex);

    struct TProxyArguments const * const arguments = arguments_;
    int const prefix_size = strlen(arguments->prefix) + 1;

    char   *error       = NULL;
    int     server_fd   = 0;
    int     client_fd   = 0;
    int     epoll_fd    = 0;

    do {
        int result = 0;

        server_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (server_fd < 0) {
            error = "server socket creation failed";
            break;
        }

        static int const yes = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
            error = "set socket option SO_REUSEADDR error";
            break;
        }

        result = bind(
            server_fd,
            (struct sockaddr const *)
                    arguments->server_address,
            sizeof(*arguments->server_address));

        if (result < 0) {
            error = "server socket bind failed";
            break;
        }

        epoll_fd = epoll_create1(0);
        if (epoll_fd < 0) {
            error = "epoll_create failed";
            break;
        }

        struct epoll_event epoll_event_subscription = {
            .events     = EPOLLIN,
            .data.fd    = server_fd
        };

        result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &epoll_event_subscription);

        if (result < 0) {
            error = "epoll_ctl: add server socket failed";
            break;
        }
    } while(0);

    is_running      = !error;
    is_initialized  = 1;

    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);

    struct epoll_event
        epoll_event_notifications[MAX_EVENTS];

    while (is_running) {
        int events_count = epoll_wait(
            epoll_fd, epoll_event_notifications, MAX_EVENTS, EPOLL_TIMEOUT);

        if (events_count < 0) {
            error = "epoll_wait failed";
            break;
        }

        for (int i = 0; i < events_count; ++i) {
            if (epoll_event_notifications[i].data.fd == server_fd) {
                struct sockaddr_in  peer_address;
                socklen_t           peer_address_size = sizeof(peer_address);
                char                buffer[BUFFER_SIZE + prefix_size];

                memset(buffer, 0, sizeof(buffer));

                int received_size = recvfrom(
                    server_fd, (char *)buffer + prefix_size, BUFFER_SIZE, MSG_WAITALL,
                    (struct sockaddr *)&peer_address, &peer_address_size);

                logger_log("received: %s", buffer + prefix_size);

                if (received_size > 0) {
                    strcpy(buffer, arguments->prefix);
                    sendto(
                        client_fd, (char const *)buffer, received_size + prefix_size,
                        MSG_CONFIRM,
                        (struct sockaddr const *)&peer_address, peer_address_size);
                }
            }

            if (epoll_event_notifications[i].data.fd == client_fd &&
                epoll_event_notifications[i].events & (EPOLLHUP | EPOLLERR))
            {
                close(client_fd);
                client_fd = 0;
            }
        }
        if (client_fd == 0)
            client_fd = connectTCPClient(arguments->client_address, epoll_fd);
    }

    close(server_fd);
    close(client_fd);

    return error;
}


pthread_t startProxy(struct TProxyArguments const * const arguments) {
    logger_log("start udp server ...");

    is_running          = 0;

    pthread_t   id      = 0;
    int         result  = pthread_create(&id, NULL, runProxyThread,
        (struct TProxyArguments * const)arguments);

    if (result) {
        logger_log("start udp server error");
        return 0;
    }

    pthread_mutex_lock(&mutex);
    while (!is_initialized)
        pthread_cond_wait(&condition, &mutex);
    pthread_mutex_unlock(&mutex);

    if (is_running) {
        logger_log("start udp server OK");
        return id;
    } else {
        logger_log("start udp server error");
        stopProxy(id);
        return 0;
    }
}


void stopProxy(pthread_t id) {
    logger_log("stop udp server ...");

    is_running      = 0;

    char *error     = NULL;
    int   result    = pthread_join(id, (void *)&error);

    if (result)
        logger_log("udp server stopping error: pthread_jpin error");
    else
        logger_log("stop udp server OK");

    if (error)
        logger_log("udp server stopped with error: %s", error);
}
