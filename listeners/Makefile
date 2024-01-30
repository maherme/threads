BIN_DIR = ./bin
CFLAGS = -O -Wall -Werror -pthread

event_listeners: listeners udp_sender

listeners: listeners.c network_utils.c network_utils.h
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $^ -o $(BIN_DIR)/$@

udp_sender: udp_sender.c network_utils.c network_utils.h
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $^ -o $(BIN_DIR)/$@

%: %.c
	@mkdir -p $(BIN_DIR)
	@gcc $(CFLAGS) $< -o $(BIN_DIR)/$@

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)
