QUIET := @
BIN_DIR = ./bin
CFLAGS = -g -O -Wall -Wextra -Werror -pthread $(INCLUDES)
DIR_THREADLIB := ../threadlib
SRC_THREADLIB := $(DIR_THREADLIB)/threadlib.c \
                $(DIR_THREADLIB)/glthreads.c
INCLUDES = -I$(DIR_THREADLIB)

main: main.c monitor_rw.c $(SRC_THREADLIB)
	$(QUIET) mkdir -p $(BIN_DIR)
	$(QUIET) gcc $(CFLAGS) $^ -o $(BIN_DIR)/$@

%: %.c
	$(QUIET) mkdir -p $(BIN_DIR)
	$(QUIET) gcc $(CFLAGS) $< -o $(BIN_DIR)/$@

.PHONY: clean
clean:
	$(QUIET) rm -rf $(BIN_DIR)
