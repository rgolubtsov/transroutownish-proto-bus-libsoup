/*
 * src/bus-controller.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.0.2
 * ============================================================================
 * A daemon written in C (GNOME/libsoup), designed and intended to be run
 * as a microservice, implementing a simple urban bus routing prototype.
 * ============================================================================
 * Copyright (C) 2023-2024 Radislav (Radicchio) Golubtsov
 *
 * (See the LICENSE file at the top of the source tree.)
 */

// The controller module of the daemon ----------------------------------------

#include "busd.h"

/**
 * Starts up the Soup web server and the main loop.
 *
 * @param server_port       The port number used to run the server.
 * @param debug_log_enabled The debug logging enabler.
 * @param routes_list       The pointer to an array containing
 *                          all available routes.
 *
 * @returns The exit code of the overall termination of the daemon.
 */
int startup(const gushort    server_port,
            const gboolean   debug_log_enabled,
            const GPtrArray *routes_list) {

    return EXIT_SUCCESS;
}

// vim:set nu et ts=4 sw=4:
