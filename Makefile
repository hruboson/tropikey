BUILD_DIR := build
BINARY    := $(BUILD_DIR)/tropikey

.PHONY: all configure build run clean rebuild

all: build

configure:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release

build: configure
	cmake --build $(BUILD_DIR) --parallel

run: build
	./$(BINARY) info.log

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean build
