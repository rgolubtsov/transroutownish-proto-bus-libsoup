/*
 * src/bus-controller.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.0.3
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
 * @returns A new <code>GMainLoop</code> main loop instance.
 */
GMainLoop *startup(const gushort    server_port,
                   const gboolean   debug_log_enabled,
                   const GPtrArray *routes_list) {

    // Creating the Soup web server and the main loop.
    SoupServer *server = soup_server_new(SERVER_HEADER, EMPTY_STRING, NULL);
    GMainLoop  *loop   = g_main_loop_new(NULL, FALSE);

    GError *error = NULL;

    // Setting up the daemon to listen on all TCP IPv4 and IPv6 interfaces.
    if (soup_server_listen_all(server, server_port,
        (SoupServerListenOptions) NULL, &error)) {

        g_message(       MSG_SERVER_STARTED, server_port);
        syslog(LOG_INFO, MSG_SERVER_STARTED, server_port);

        // Starting up the daemon by running the main loop.
        g_main_loop_run(loop);
    } else {
        g_clear_error(&error);
    }

    return loop;
}

// vim:set nu et ts=4 sw=4:
