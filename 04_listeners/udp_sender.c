#include "network_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define print_error_and_exit(msg) \
        do { fprintf(stderr, msg); exit(EXIT_FAILURE); } while(0)

int
main(int argc, char **argv)
{
    in_port_t port_number;
    long port_number_long;
    char *endptr;

    if(argc > 4)
    {
        print_error_and_exit("Wrong arguments number\n");
    }

    port_number_long = strtol(argv[2], &endptr, 10);
    if('\0' != *endptr || port_number_long < 0 || port_number_long > 65535)
    {
        print_error_and_exit("Invalid port number\n");
    }
    port_number = (in_port_t)port_number_long;

    if(is_ipv4_valid(argv[1]))
    {
        print_error_and_exit("Invalid IP address\n");
    }

    printf("Dest = [%s,%d]\n", argv[1], port_number);
    send_udp_msg(argv[1], port_number, argv[3], strlen(argv[3]));

    exit(EXIT_SUCCESS);
}
