BIN_DIR := ./bin
CFLAGS := -O -Wall -Werror
LDFLAGS := -pthread
VPATH := ./

mutex: $(BIN_DIR)/mutex.o
	@mkdir -p $(BIN_DIR)
	gcc $^ -o $(BIN_DIR)/$@ $(LDFLAGS)

$(BIN_DIR)/%.o: $(BIN_DIR)/%.s
	@mkdir -p $(BIN_DIR)
	as $< -o $@

.PRECIOUS: $(BIN_DIR)/%.s
$(BIN_DIR)/%.s: %.c
	@mkdir -p $(BIN_DIR)
	gcc -S $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)
