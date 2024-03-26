#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "proxy.h"
#include "logger.h"


struct sockaddr_in *socket_address_make(char *str) {
    char *ip    = strtok(str,  ":");
    char *port  = strtok(NULL, ":");

    if (ip && port) {
        struct sockaddr_in *address = calloc(1, sizeof(struct sockaddr_in));

        if (!address) {
            logger_log("memory allocation error");
            return NULL;
        }

        address->sin_family = AF_INET;
        address->sin_port   = htons(atoi(port));

        if (inet_pton(AF_INET, ip, &address->sin_addr) <= 0) {
            free(address);
            address = NULL;
            logger_log("ip:port address convertion error");
        }

        return address;
    } else
        logger_log("wrong format ip:port '%s'", str);

    return NULL;
}


int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("%s server_ip:port client_ip:port lof_file_path prefix4\n", argv[0]);
        return 1;
    }

    int exit_code   = 0;
    int logger      = logger_start(argv[3]);

    logger_log("std         : %ld", __STDC_VERSION__);
    logger_log("server      : %s" , argv[1]);
    logger_log("client      : %s" , argv[2]);
    logger_log("log_path    : %s" , argv[3]);
    logger_log("prefix      : %s" , argv[4]);

    struct TProxyArguments arguments = {
        .server_address = socket_address_make(argv[1]),
        .client_address = socket_address_make(argv[2]),
        .prefix         = argv[4]
    };

    if (arguments.server_address && arguments.client_address) {
        pthread_t id = startProxy(&arguments);
        if (id) {
            logger_log("press enter to stop server");
            getchar();
            stopProxy(id);
        }
    } else
        exit_code = EXIT_FAILURE;

    if (logger)
        logger_stop();

    free(arguments.server_address);
    free(arguments.client_address);

    return exit_code;
}
