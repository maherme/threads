#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include "network_utils.h"

#define MAX_PACKET_BUFFER_SIZE      64
#define IP_ADDR_SIZE                16
#define NUMBER_FIELDS_IPV4          4
#define MAX_DIGITS_IN_IP_FIELD      3
#define MAX_DOTS_IN_IP_ADDR         3
#define MAX_STRING_SIZE_IP_FIELD    4

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define print_error_and_exit(msg) \
        do { fprintf(stderr, msg); exit(EXIT_FAILURE); } while(0)
#define thread_handle_error_en(en, msg) \
        do { errno = en; perror(msg); pthread_exit(NULL); } while (0)
#define thread_handle_error(msg) \
        do { perror(msg); pthread_exit(NULL); } while (0)
#define thread_print_error_and_exit(msg) \
        do { fprintf(stderr, msg); pthread_exit(NULL); } while(0)

typedef struct
{
    char ip_addr[IP_ADDR_SIZE];
    int port_number;
    recv_fn_cb receiver_callback;
} thread_arg_packet_t;

static void *
server_thread_start(void *arg)
{
    thread_arg_packet_t *thread_arg_packet = (thread_arg_packet_t *)arg;
    char ip_addr[16];
    int port_number = thread_arg_packet->port_number;
    recv_fn_cb recv_fn = thread_arg_packet->receiver_callback;
    int udp_socket_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char *receiver_buffer;
    ssize_t bytes_received;
    unsigned int addr_len = sizeof(client_addr);

    strncpy(ip_addr, thread_arg_packet->ip_addr, IP_ADDR_SIZE);

    free(thread_arg_packet);
    thread_arg_packet = NULL;

    udp_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(-1 == udp_socket_fd)
    {
        thread_handle_error("socket");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port_number;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(-1 == bind(udp_socket_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)))
    {
        thread_handle_error("bind");
    }

    receiver_buffer = calloc(1, MAX_PACKET_BUFFER_SIZE);
    if(!receiver_buffer)
    {
        handle_error("calloc");
    }

    while(1)
    {
        memset(receiver_buffer, 0, MAX_PACKET_BUFFER_SIZE);

        bytes_received = recvfrom(udp_socket_fd, receiver_buffer,
                                  MAX_PACKET_BUFFER_SIZE, 0,
                                  (struct sockaddr *) &client_addr, &addr_len);
        if(-1 == bytes_received)
        {
            thread_handle_error("recvfrom");
        }

        recv_fn(receiver_buffer, bytes_received,
                network_convert_ip_n_to_p((uint32_t)htonl(client_addr.sin_addr.s_addr), 0),
                client_addr.sin_port);
    }

    return NULL;
}

pthread_t *
udp_server_create_and_start(char *ip_addr, int udp_port_number, recv_fn_cb receiver_callback)
{
    pthread_attr_t attr;
    pthread_t *receiver_packet_thread;
    thread_arg_packet_t *thread_arg_packet;
    int s;

    s = pthread_attr_init(&attr);
    if(0 != s)
    {
        handle_error_en(s, "pthread_attr_init");
    }

    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(0 != s)
    {
        handle_error_en(s, "pthread_attr_setdetachstate");
    }

    thread_arg_packet = calloc(1, sizeof(thread_arg_packet_t));
    if(!thread_arg_packet)
    {
        handle_error("calloc");
    }

    strncpy(thread_arg_packet->ip_addr, ip_addr, IP_ADDR_SIZE);
    thread_arg_packet->port_number = udp_port_number;
    thread_arg_packet->receiver_callback = receiver_callback;

    receiver_packet_thread = calloc(1, sizeof(pthread_t));
    if(!receiver_packet_thread)
    {
        handle_error("calloc");
    }

    s = pthread_create(receiver_packet_thread, &attr, server_thread_start, (void*)thread_arg_packet);
    if(0 != s)
    {
        handle_error_en(s, "pthread_create");
    }

    return receiver_packet_thread;
}

int
send_udp_msg(char *dest_ip, in_port_t dest_port_number, char *msg, size_t msg_size)
{
    struct sockaddr_in dest;
    struct hostent *host;
    int sock_fd;
    ssize_t num_sent_bytes;

    dest.sin_family = AF_INET;
    dest.sin_port = dest_port_number;
    host = (struct hostent *)gethostbyname(dest_ip);
    dest.sin_addr = *((struct in_addr *)host->h_addr);

    sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock_fd < 0)
    {
        handle_error("socket");
    }

    num_sent_bytes = sendto(sock_fd, msg, msg_size, 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
    if(-1 == num_sent_bytes)
    {
        handle_error("sendto");
    }

    close(sock_fd);

    return num_sent_bytes;
}

char *
network_convert_ip_n_to_p(uint32_t ip_addr, char *output)
{
    char *result;
    static char str_ip[16];

    result = !output ? str_ip : output;

    memset(result, 0, 16);
    ip_addr = htonl(ip_addr);
    inet_ntop(AF_INET, &ip_addr, result, 16);
    result[15] = '\0';

    return result;
}

int
is_ipv4_valid(const char *ip)
{
    char string_ip_field[MAX_STRING_SIZE_IP_FIELD] = {'\0'};
    int digit_counter_in_ip_field = 0;
    int dot_counter_in_ip = 0;
    int int_ip_field;
    char *endptr;
    size_t len_string_ip;

    len_string_ip = strlen(ip);
    if(16 < len_string_ip)
    {
        return 1;
    }

    for(size_t i = 0; i < (len_string_ip + 1); i++)
    {
        if(('\0' == ip[i]) && (MAX_DOTS_IN_IP_ADDR != dot_counter_in_ip))
        {
            return 1;
        }

        if(('.' == ip[i]) || ('\0' == ip[i]))
        {
            int_ip_field = strtol(string_ip_field, &endptr, 10);
            if(('\0' != *endptr) || (string_ip_field == endptr))
            {
                return 1;
            }
            if((0 > int_ip_field) || (255 < int_ip_field))
            {
                return 1;
            }
            dot_counter_in_ip++;
            digit_counter_in_ip_field = 0;
            for(int i = 0; i < MAX_STRING_SIZE_IP_FIELD; i++)
            {
                string_ip_field[i] = '\0';
            }
        }
        else if(MAX_DIGITS_IN_IP_FIELD < digit_counter_in_ip_field)
        {
            return 1;
        }
        else
        {
            string_ip_field[digit_counter_in_ip_field] = ip[i];
            digit_counter_in_ip_field++;
        }
    }

    return 0;
}
