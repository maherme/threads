BIN_DIR = ./bin
CFLAGS = -Wall -Werror
SOURCES = main.c glthreads.c notification_chain.c routing_table.c threated_publisher.c threated_subscriber.c
INCLUDES = glthreads.h notification_chain.h routing_table.h utilities.h threated_publisher.h threated_subscriber.h

all: main

main: $(SOURCES) $(INCLUDES)
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $^ -o $(BIN_DIR)/$@

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)
