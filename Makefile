#
# Makefile
# =============================================================================
# Urban bus routing microservice prototype (C port). Version 0.0.1
# =============================================================================
# A daemon written in C (GNOME/libsoup), designed and intended to be run
# as a microservice, implementing a simple urban bus routing prototype.
# =============================================================================
# Copyright (C) 2023 Radislav (Radicchio) Golubtsov
#
# (See the LICENSE file at the top of the source tree.)
#

BIN_DIR = bin
SRC_DIR = src

PREF = busd
EXEC = $(BIN_DIR)/$(PREF)
DEPS = $(SRC_DIR)/$(PREF).o

# Specify flags and other vars here.
MKDIR   = mkdir
RMFLAGS = -vR

# Making the first target (object files).
$(DEPS): %.o: %.c

CFLAGS=-o $(EXEC)

# Making the second target (the microservice itself).
$(EXEC): $(DEPS)
	if [ ! -d $(BIN_DIR) ]; then \
	    $(MKDIR) $(BIN_DIR); \
	fi
	$(CC) $(CFLAGS) $(DEPS)

.PHONY: all clean

all: $(EXEC)

clean:
	$(RM) $(RMFLAGS) $(BIN_DIR) $(DEPS)

# vim:set nu et ts=4 sw=4: