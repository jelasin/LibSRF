.PHONY: all clean

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(BIN_DIR)/utils/%.o)

all: $(OBJS)

$(BIN_DIR)/utils/%.o: %.c
	@[ -d $(BIN_DIR)/utils ] || mkdir -p $(BIN_DIR)/utils
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BIN_DIR)/utils