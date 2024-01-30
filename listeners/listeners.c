#include <pthread.h>
#include <stdio.h>
#include "network_utils.h"

pthread_t *listener1 = NULL;
pthread_t *listener2 = NULL;

void
packet_receive(char *packet, int packet_size, char *sender_ip, int port_number)
{
    printf("%s() : packet received = %s, packet size = %d\n", __FUNCTION__, packet, packet_size);
}

int
main(int argc, char **argv)
{
    printf("Listening on UDP port no 3000\n");
    listener1 = udp_server_create_and_start("127.0.0.1", 3000, packet_receive);

    printf("Listening on UDP port no 3001\n");
    listener2 = udp_server_create_and_start("127.0.0.1", 3001, packet_receive);

    pthread_exit(NULL);
}
