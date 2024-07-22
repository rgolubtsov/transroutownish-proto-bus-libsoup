/*
 * src/bus-controller.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.1.2
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
 * @param cleanup_args      The pointer to a structure that holds arguments
 *                          for the <code>_cleanup()</code> helper function.
 *
 * @returns A new <code>GMainLoop</code> main loop instance.
 */
GMainLoop *startup(const gushort        server_port,
                   const gboolean       debug_log_enabled,
                   const GPtrArray     *routes_list,
                         _CLEANUP_ARGS *cleanup_args) {

    // Creating the Soup web server and the main loop.
    SoupServer *server = soup_server_new(HDR_SERVER_P, EMPTY_STRING, NULL);
    GMainLoop  *loop   = g_main_loop_new(NULL, FALSE);

    cleanup_args->loop = loop;

    if (server == NULL) {
        g_warning(ERR_CANNOT_START_SERVER ERR_SERV_UNKNOWN_REASON);

        _cleanup(cleanup_args);
        free(cleanup_args);

        exit(EXIT_FAILURE);
    }

    // Attaching Unix signal handlers to ensure daemon clean shutdown.
    g_unix_signal_add(SIGINT,  (GSourceFunc) _cleanup, cleanup_args);
    g_unix_signal_add(SIGTERM, (GSourceFunc) _cleanup, cleanup_args);

    // Attaching HTTP request handlers to process incoming requests.
    soup_server_add_handler(server, NULL, request_handler, NULL, NULL);

    GError *error = NULL;

    // Setting up the daemon to listen on all TCP IPv4 and IPv6 interfaces.
    if (soup_server_listen_all(server, server_port,
        (SoupServerListenOptions) NULL, &error)) {

        g_message(       MSG_SERVER_STARTED, server_port);
        syslog(LOG_INFO, MSG_SERVER_STARTED, server_port);

        // Starting up the daemon by running the main loop.
        g_main_loop_run(loop);
    } else {
        if (error->code == ERR_EADDRINUSE_CODE) {
            g_warning(ERR_CANNOT_START_SERVER ERR_ADDR_ALREADY_IN_USE);
        }

        g_clear_error(&error);

        _cleanup(cleanup_args);
        free(cleanup_args);

        exit(EXIT_FAILURE);
    }

    return loop;
}

// vim:set nu et ts=4 sw=4:
