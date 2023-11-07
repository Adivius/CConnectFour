BUILD_DIR = build
TARGET = cconnectfour

SRC_DIR = src
INCLUDE_DIR = $(SRC_DIR)

CC = gcc
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf

CFLAGS = -O2 -I$(INCLUDE_DIR)
LDFLAGS = -s -flto -fuse-ld=gold $(LIBS)

#Autodetect all necessary files
SOURCES = $(shell find $(SRC_DIR) -name *.c)
OBJS = $(SOURCES:%=$(BUILD_DIR)/obj/%.o)

.PHONY: all format clean

#Create the target file
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

#Compile source files into object files
$(BUILD_DIR)/obj/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

all: format $(BUILD_DIR)/$(TARGET)

format:
	clang-format -i `find $(SRC_DIR) $(INCLUDE_DIR) -name \*.c -or -name \*.h`

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean $(BUILD_DIR)/$(TARGET)
