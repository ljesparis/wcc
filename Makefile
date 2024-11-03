.PHONY: run clean

BUILD ?= ${PWD}/build

CC = gcc

CFLAGS=-I${PWD}/include
SOURCE = $(wildcard ${PWD}/*.c)

clean:
	@rm -rf $(BUILD)

create_build_dir: clean
	@mkdir $(BUILD) 2>/dev/null; true

build: create_build_dir
	@$(CC) -g -O3 $(CFLAGS) $(SOURCE) -o $(BUILD)/wcc

