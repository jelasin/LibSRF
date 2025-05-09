.PHONY: $(SUB_DIRS) all clean

SUB_DIRS = $(shell ls -d */ 2>/dev/null)

all: $(BIN_DIR)/main.o
	$(CC) $(CFLAGS) -o $(TARGET) $(BIN_DIR)/main.o $(foreach dir,$(SUB_DIRS),$(wildcard $(BIN_DIR)/$(dir)/*.o))

$(BIN_DIR)/main.o: main.c
	$(foreach dir,$(SUB_DIRS),$(MAKE) -C $(SRC_DIR)/$(dir) all;)
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/main.o main.c

clean:
	$(foreach dir,$(SUB_DIRS),$(MAKE) -C $(SRC_DIR)/$(dir) clean;)
	@rm -f $(BIN_DIR)/main.o