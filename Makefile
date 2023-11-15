BUILD_DIR=build
CC = clang++

PS = %

################################################################################
# Dependency graph.
################################################################################

# Usage:
# graph.module = module_0 module_1 module_2

################################################################################
# Compilation flags.
################################################################################

CFLAGS_MODULE =\
 -x c++-module -fmodule-output

CFLAGS =\
 -stdlib=libc++\
 -Wall -Wextra -Wno-missing-field-initializers -O2 -std=c++23 --pedantic\
 -fprebuilt-module-path=$(BUILD_DIR) -Isrc\
 $(shell pkg-config --cflags sdl2)\
 $(shell pkg-config --cflags glew)

CLIBS =\
 -lc++\
 $(shell pkg-config --libs sdl2)\
 $(shell pkg-config --libs glew)

################################################################################
# Source file list.
################################################################################

FILES_SRC = $(sort $(wildcard src/*.ccm))

################################################################################
# Object file list.
################################################################################

OBJECT_FILES =\
 $(patsubst src/%.ccm,$(BUILD_DIR)/%.o,$(FILES_SRC))

################################################################################
# Targets.
################################################################################

all: $(patsubst src/%.cc,$(BUILD_DIR)/program.%,$(sort $(wildcard src/*.cc)))

clean:
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BUILD_DIR)/*.pcm
	rm -f $(BUILD_DIR)/program.*

$(BUILD_DIR)/program.%: $(OBJECT_FILES) $(BUILD_DIR)/%.o
	$(CC) -Wl,--allow-multiple-definition $^ $(CLIBS) -o $@

.SECONDEXPANSION:

$(BUILD_DIR)/%.o: src/%.cc $$(patsubst $$(PS),$$(BUILD_DIR)/$$(PS).o,$$(graph.$$(patsubst $$(BUILD_DIR)/$$(PS).o,$$(PS),$$@)))
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: src/%.ccm $$(patsubst $$(PS),$$(BUILD_DIR)/$$(PS).o,$$(graph.$$(patsubst $$(BUILD_DIR)/$$(PS).o,$$(PS),$$@)))
	$(CC) $(CFLAGS) $(CFLAGS_MODULE) -c -o $@ $<
