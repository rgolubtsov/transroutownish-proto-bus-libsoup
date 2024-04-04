/*
 * src/bus-helper.c
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

// The helper module for the daemon -------------------------------------------

#include "busd.h"

/**
 * Retrieves the port number used to run the server, from daemon settings.
 *
 * @param settings The pointer to a structure containing key-value pairs
 *                 of individual settings.
 *
 * @return The port number on which the server has to be run.
 */
unsigned short get_server_port(GKeyFile *settings) {
    GError *error = NULL;

    unsigned short server_port
        = g_key_file_get_integer(settings, SERVER_GROUP, SERVER_PORT, &error);

    if (server_port == 0) {
        server_port = DEF_PORT;
    }

    return server_port;
}

/**
 * Identifies whether debug logging is enabled by retrieving
 * the corresponding setting from daemon settings.
 *
 * @param settings The pointer to a structure containing key-value pairs
 *                 of individual settings.
 *
 * @return <code>TRUE</code> if debug logging is enabled,
 *         <code>FALSE</code> otherwise.
 */
gboolean is_debug_log_enabled(GKeyFile *settings) {
    GError *error = NULL;

    gboolean debug_log_enabled
        = g_key_file_get_boolean(settings, LOGGER_GROUP, LOG_ENABLED, &error);

    return debug_log_enabled;
}

/**
 * Retrieves the path and filename of the routes data store
 * from daemon settings.
 *
 * @param settings The pointer to a structure containing key-value pairs
 *                 of individual settings.
 *
 * @return The path and filename of the routes data store
 *         or <code>NULL</code>, if they are not defined.
 */
gchar *get_routes_datastore(GKeyFile *settings) {
    GError *error = NULL;

    gchar *path_prefix
        = g_key_file_get_string(settings, ROUTES_GROUP, PATH_PREFIX, &error);
    error = NULL;
    gchar *path_dir
        = g_key_file_get_string(settings, ROUTES_GROUP, PATH_DIR,    &error);
    error = NULL;
    gchar *filename
        = g_key_file_get_string(settings, ROUTES_GROUP, FILENAME,    &error);

    gchar *datastore = EMPTY_STRING;

    if (path_prefix != NULL) {
        datastore = g_strconcat(datastore, path_prefix, NULL); }
    if (path_dir    != NULL) {
        datastore = g_strconcat(datastore, path_dir,    NULL); }
    if (filename    != NULL) {
        datastore = g_strconcat(datastore, filename,    NULL); }

    g_free(filename   );
    g_free(path_dir   );
    g_free(path_prefix);

    if (g_utf8_strlen(datastore, -1) == 0) { return NULL; }

    return datastore;
}

// Helper function. Used to get the daemon settings.
GKeyFile *_get_settings() {
    GKeyFile *settings = g_key_file_new();
    GError   *error    = NULL;

    gboolean is_loaded = g_key_file_load_from_file(settings, SETTINGS,
        G_KEY_FILE_NONE, &error);

    if (!is_loaded) {
        gboolean is_failed
            = g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT);

        if (is_failed) {
            g_warning(ERR_SETTINGS_NOT_FOUND, error->message);
        }

        return NULL;
    }

    return settings;
}

// vim:set nu et ts=4 sw=4:
