#
# Makefile
# =============================================================================
# Urban bus routing microservice prototype (C port). Version 0.0.1
# =============================================================================
# A daemon written in C (GNOME/libsoup), designed and intended to be run
# as a microservice, implementing a simple urban bus routing prototype.
# =============================================================================
# Copyright (C) 2023-2024 Radislav (Radicchio) Golubtsov
#
# (See the LICENSE file at the top of the source tree.)
#

BIN_DIR = bin
SRC_DIR = src

PREF = bus
EXEC = $(BIN_DIR)/$(PREF)d
DEPS = $(SRC_DIR)/$(PREF)-core.o \
       $(SRC_DIR)/$(PREF)-helper.o

# Specify flags and other vars here.
CSTD   = c99
CFLAGS = -Wall -pedantic -std=$(CSTD) -march=x86-64 -O3 -pipe

MKDIR   = mkdir
RMFLAGS = -vR

CFLAGS += `pkg-config --cflags-only-I glib-2.0`
LDLIBS  = `pkg-config   --libs-only-l glib-2.0`

# Making the first target (object files).
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

CFLAGS += -o $(EXEC)

# Making the second target (the microservice itself).
$(EXEC): $(DEPS)
	if [ ! -d $(BIN_DIR) ]; then \
	    $(MKDIR) $(BIN_DIR); \
	fi
	$(CC) $(CFLAGS) $(LDLIBS) $(DEPS)

.PHONY: all clean

all: $(EXEC)

clean:
	$(RM) $(RMFLAGS) $(BIN_DIR) $(DEPS)

# vim:set nu et ts=4 sw=4:
