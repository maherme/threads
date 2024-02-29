#ifndef THREATED_PUBLISHER_H
#define THREATED_PUBLISHER_H

#include "routing_table.h"

void
publisher_create_thread(void);

void
publisher_init_routing_table(void);

routing_table_t *
publisher_get_routing_table(void);

#endif /* THREATED_PUBLISHER_H */
