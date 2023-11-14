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
int main(int argc, char *const *argv) {
    char *daemon_name = argv[0];
    unsigned short server_port;

    if (argc > 1) {
        server_port = atoi(argv[1]);
    } else {
        server_port = DEF_PORT;
    }

    printf(MSG_SERVER_STARTED NEW_LINE, daemon_name, server_port);
    printf(MSG_SERVER_STOPPED NEW_LINE, daemon_name);
}

// vim:set nu et ts=4 sw=4:
