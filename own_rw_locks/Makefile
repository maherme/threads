BIN_DIR = ./bin
CFLAGS = -g -O -Wall -Wextra -Werror -pthread

main: main.c threads.c rw_locks.c
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $^ -o $(BIN_DIR)/$@

%: %.c
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $< -o $(BIN_DIR)/$@

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)
