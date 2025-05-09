.PHONY: all clean distclean

export SRC_DIR=$(PWD)/Src
export INC_DIR=$(PWD)/Include
export BIN_DIR=$(PWD)/Bin

export CC=gcc

export debug=true
ifeq ($(debug), true)
export CFLAGS=-g -Wall -Wextra -Wpedantic -I$(INC_DIR)/utils -I$(INC_DIR)/shmfifo -DDEBUG
else
export CFLAGS=-O3 -Wall -Wextra -I$(INC_DIR)/utils -I$(INC_DIR)/shmfifo -Wpedantic
endif

export TARGET = $(BIN_DIR)/main

all: $(TARGET)

$(TARGET):
	[ -d $(BIN_DIR) ] || mkdir $(BIN_DIR)
	$(MAKE) -C $(SRC_DIR)

clean:
	$(MAKE) -C $(SRC_DIR) clean

distclean:
	rm -rf $(BIN_DIR)/*


