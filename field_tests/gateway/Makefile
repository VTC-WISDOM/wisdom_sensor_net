MAKEFLAGS += --no-print-directory
SHELL := /bin/bash

# Pull in target from cmake config file
target = ${shell cat wisdom_config.cmake | grep "set(target" | sed -E 's/.*"(.*)".*/\1/'}
uf2 = build/$(target).uf2

default:
	@echo "Makefile: no default target"

build: clean
	mkdir -p build
	cd build; cmake ..; $(MAKE) -j8

load:
	sudo picotool load $(uf2) -fx

clean:
	rm -rf build

.PHONY: build load clean
