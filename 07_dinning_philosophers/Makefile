BIN_DIR = ./bin
CFLAGS = -O -Wall -Wextra -Werror -pthread

main: main.c threads.c dinner_phi.c
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $^ -o $(BIN_DIR)/$@

%: %.c
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $< -o $(BIN_DIR)/$@

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)
