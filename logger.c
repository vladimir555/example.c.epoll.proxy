#include "logger.h"

#include <stdio.h>
#include <pthread.h>
#include <stdarg.h>


static FILE            *file   = NULL;
static pthread_mutex_t  mutex  = PTHREAD_MUTEX_INITIALIZER;


int logger_start(char const * const path) {
    pthread_mutex_lock(&mutex);
    if (file)
        perror("logger initialization error: already initialized");
    else
        file = fopen(path, "a");
    _Bool result = file;
    pthread_mutex_unlock(&mutex);

    return result;
}


void logger_stop() {
    if (file) {
        pthread_mutex_lock(&mutex);
        fclose(file);
        file = NULL;
        pthread_mutex_unlock(&mutex);
    } else {
        perror("logger finalization error: not initialized");
        return;
    }
}


void logger_log(const char *__restrict __format, ...) {
    va_list args;
    va_start(args, __format);

    vprintf(__format, args);
    printf("\n");

    pthread_mutex_lock(&mutex);
    if (file) {
        vfprintf(file, __format, args);
        fprintf(file, "\n");
        fflush(file);
    }
    pthread_mutex_unlock(&mutex);

    va_end(args);
}
