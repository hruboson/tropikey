BUILD_DIR 		:= build
PROJECT			:= tropikey

BINARY_NAME		:= $(PROJECT)
BINARY    		:= $(BUILD_DIR)/$(BINARY_NAME)

PKCS_SO_NAME	:= $(PROJECT)_pkcs11.so
PKCS_SO    		:= $(BUILD_DIR)/$(PKCS_SO_NAME)

.PHONY: all configure build run clean rebuild

all: build

configure:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release

build: configure
	cmake --build $(BUILD_DIR) --parallel
	cp $(BINARY) ./$(BINARY_NAME)
	cp $(PKCS_SO) ./$(PKCS_SO_NAME)

run: build
	./$(BINARY) info.log

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean build
