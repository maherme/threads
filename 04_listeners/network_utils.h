#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <pthread.h>
#include <stdint.h>
#include <netinet/in.h>

typedef void (*recv_fn_cb)(char *packet, int packet_size, char *sender_ip, int port_number);

pthread_t *
udp_server_create_and_start(char *ip_addr, int udp_port_number, recv_fn_cb receiver_callback);

char *
network_convert_ip_n_to_p(uint32_t ip_addr, char *output);

int
send_udp_msg(char *dest_ip, in_port_t dest_port_number, char *msg, size_t msg_size);

int
is_ipv4_valid(const char *ip);

#endif /* NETWORK_UTILS_H */
