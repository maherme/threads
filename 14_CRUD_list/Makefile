BIN_DIR = ./bin
CFLAGS = -g -O -Wall -Wextra -Werror -pthread

main: main.c threads.c ref_count.c linked_list.c student_list.c
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $^ -o $(BIN_DIR)/$@

%: %.c
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $< -o $(BIN_DIR)/$@

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)
