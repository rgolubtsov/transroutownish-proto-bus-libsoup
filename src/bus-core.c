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

    // Creating the log directory.
    GFile *logdir = g_file_new_for_path(LOG_DIR);
    g_file_make_directory(logdir, NULL, NULL); g_object_unref(logdir);

    // Creating the logfile and associating
    // a newly allocated output stream with it.
    GFile *logfile = g_file_new_for_path(LOG_DIR LOGFILE);
    GFileOutputStream *log_stream = g_file_append_to(logfile,
        G_FILE_CREATE_NONE, NULL, NULL);

    // Registering the log writer callback.
    g_log_set_writer_func(log_writer, log_stream, NULL);

    // Opening the system logger.
    openlog(NULL, LOG_CONS | LOG_PID, LOG_DAEMON);

    // Getting the daemon settings.
    GKeyFile *settings = _get_settings();

    unsigned short server_port = DEF_PORT;
    gboolean debug_log_enabled __attribute__ ((unused)) = TRUE;
    gchar *datastore = EMPTY_STRING;

    if (settings != NULL) {
        // Getting the port number used to run the server,
        // from daemon settings.
        server_port = get_server_port(settings);

        // Identifying whether debug logging is enabled.
        debug_log_enabled = is_debug_log_enabled(settings);

        // Getting the path and filename of the routes data store
        // from daemon settings.
        datastore = get_routes_datastore(settings);

        g_free(settings);
    }

    if ((datastore == NULL) || (g_utf8_strlen(datastore, -1) == 0)) {
        datastore = g_strdup(SAMPLE_ROUTES);
    }

    GFile *data = g_file_new_for_path(datastore);

    if (!g_file_query_exists(data, NULL)) {
        g_message(ERR_DATASTORE_NOT_FOUND);

        g_object_unref(data);
        g_free(datastore);

        _cleanup(log_stream, logfile);

        exit(EXIT_FAILURE);
    }

    g_object_unref(data);

    printf(MSG_SERVER_STARTED NEW_LINE,  daemon_name, server_port);
    syslog(LOG_INFO, MSG_SERVER_STARTED, daemon_name, server_port);

    g_free(datastore);

    printf(MSG_SERVER_STOPPED NEW_LINE,  daemon_name);
    syslog(LOG_INFO, MSG_SERVER_STOPPED, daemon_name);

    _cleanup(log_stream, logfile);
}

// vim:set nu et ts=4 sw=4:
