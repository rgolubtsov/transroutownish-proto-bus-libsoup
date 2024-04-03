/*
 * src/bus-core.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.0.1
 * ============================================================================
 * A daemon written in C (GNOME/libsoup), designed and intended to be run
 * as a microservice, implementing a simple urban bus routing prototype.
 * ============================================================================
 * Copyright (C) 2023-2024 Radislav (Radicchio) Golubtsov
 *
 * (See the LICENSE file at the top of the source tree.)
 */

// The main module of the daemon ----------------------------------------------

#include "busd.h"

/**
 * The microservice entry point.
 *
 * @param argc The number of command-line arguments + 1 (the daemon name).
 * @param argv The pointer to an array of command-line arguments,
 *             including the daemon name.
 *
 * @returns The exit code of the overall termination of the daemon.
 */
int main(int argc, char *const *argv) {
    char *daemon_name = argv[0];

/*  if (argc > 1) {
        server_port = atoi(argv[1]);
    } else {
        server_port = DEF_PORT;
    }*/

    // Getting the daemon settings.
    GKeyFile *settings = _get_settings();

    unsigned short server_port = DEF_PORT;
    gboolean debug_log_enabled = TRUE;

    if (settings != NULL) {
        printf("=== Settings loaded ===" NEW_LINE);

        // Getting the port number used to run the server,
        // from daemon settings.
        server_port = get_server_port(settings);

        // Identifying whether debug logging is enabled.
        debug_log_enabled = is_debug_log_enabled(settings);
    }

    g_free(settings);

    printf(MSG_SERVER_STARTED NEW_LINE, daemon_name, server_port);
    printf("=== %u ===" NEW_LINE, debug_log_enabled);
    printf(MSG_SERVER_STOPPED NEW_LINE, daemon_name);
}

// vim:set nu et ts=4 sw=4:
