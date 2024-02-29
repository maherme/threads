#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define print_error_and_exit(msg) \
        do { fprintf(stderr, msg); exit(EXIT_FAILURE); } while(0)

#endif /* UTILITIES_H */
