QUIET := @
BIN_DIR := ./bin
CFLAGS = -O -Wall -Wextra -Werror -pthread $(INCLUDES)
DIR_THREADLIB := ../threadlib
SRC_THREADLIB := $(DIR_THREADLIB)/threadlib.c \
                $(DIR_THREADLIB)/glthreads.c
INCLUDES := -I$(DIR_THREADLIB)

main: main.c thread_barrier.c $(SRC_THREADLIB)
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $^ -o $(BIN_DIR)/$@

%: %.c
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $< -o $(BIN_DIR)/$@

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)
