.PHONY: all clean

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(BIN_DIR)/shmfifo/%.o)

all: $(OBJS)

$(BIN_DIR)/shmfifo/%.o: %.c
	@[ -d $(BIN_DIR)/shmfifo ] || mkdir -p $(BIN_DIR)/shmfifo
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BIN_DIR)/shmfifo