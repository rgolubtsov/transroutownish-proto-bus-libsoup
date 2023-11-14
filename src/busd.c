/*
 * src/busd.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.0.1
 * ============================================================================
 * A daemon written in C (GNOME/libsoup), designed and intended to be run
 * as a microservice, implementing a simple urban bus routing prototype.
 * ============================================================================
 * Copyright (C) 2023 Radislav (Radicchio) Golubtsov
 *
 * (See the LICENSE file at the top of the source tree.)
 */

#include "busd.h"

// The microservice entry point.
int main() {
    puts(MSG_SERVER_STARTED);
    puts(MSG_SERVER_STOPPED);
}

// vim:set nu et ts=4 sw=4:
