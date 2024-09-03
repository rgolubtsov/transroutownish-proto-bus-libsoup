/*
 * src/bus-helper.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.2.5
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
 * The log writer callback. Gets called on every message logging attempt.
 *
 * @param log_level The log level of the message.
 * @param fields    An array of fields forming the message.
 * @param n_fields  The number of <code>fields</code>.
 * @param user_data An arbitrary data that is passed
 *                  to the <code>g_log_set_writer_func()</code> setter.
 *
 * @return <code>G_LOG_WRITER_HANDLED</code> if the log entry was handled
 *         successfully, <code>G_LOG_WRITER_UNHANDLED</code> otherwise.
 */
GLogWriterOutput log_writer(      GLogLevelFlags  log_level,
                            const GLogField      *fields,
                                  gsize           n_fields,
                                  gpointer        user_data) {

    GFileOutputStream *log_stream = user_data;

    for (gsize i = 0; i < n_fields; i++) {
        if (g_strcmp0(fields[i].key, LOG_KEY_MESSAGE) == 0) {
            FILE  *stream = NULL;
            gchar *llevel = NULL;

            if (log_level == G_LOG_LEVEL_WARNING) {
                stream = stderr;
                llevel = LOG_LEVEL_WARN;
            }

            if (log_level == G_LOG_LEVEL_DEBUG) {
                stream = stdout;
                llevel = LOG_LEVEL_DEBUG;
            }

            if (log_level == G_LOG_LEVEL_MESSAGE) {
                stream = stdout;
                llevel = LOG_LEVEL_INFO;
            }

GDateTime *date_time = g_date_time_new_now_local();
gchar *year   = g_strdup_printf(DTM_FORMAT, g_date_time_get_year        (date_time));
gchar *month  = g_strdup_printf(DTM_FORMAT, g_date_time_get_month       (date_time));
gchar *day    = g_strdup_printf(DTM_FORMAT, g_date_time_get_day_of_month(date_time));
gchar *hour   = g_strdup_printf(DTM_FORMAT, g_date_time_get_hour        (date_time));
gchar *minute = g_strdup_printf(DTM_FORMAT, g_date_time_get_minute      (date_time));
gchar *second = g_strdup_printf(DTM_FORMAT, g_date_time_get_second      (date_time));

            gchar *message = g_strconcat("[", year,
                                         "-", month,
                                         "-", day,
                                        "][", hour,
                                         ":", minute,
                                         ":", second,
                                        "][", llevel,
            (log_level == G_LOG_LEVEL_DEBUG)
                   ? (      "]" SPACE SPACE)
                   : (SPACE "]" SPACE SPACE), fields[i].value, NEW_LINE, NULL);

            // Writing the log message to an output stream.
            fprintf(stream, LOG_FORMAT, message);

            // Writing the log message to a logfile.
            gssize nbytes = g_output_stream_write((GOutputStream *) log_stream,
                message, strlen(message), NULL, NULL);

            g_free(message);

            g_free(second);
            g_free(minute);
            g_free(hour  );
            g_free(day   );
            g_free(month );
            g_free(year  );

            if (nbytes == -1) { return G_LOG_WRITER_UNHANDLED; }
        }
    }

    return G_LOG_WRITER_HANDLED;
}

/**
 * Retrieves the port number used to run the server, from daemon settings.
 *
 * @param settings The pointer to a structure containing key-value pairs
 *                 of individual settings.
 *
 * @return The port number on which the server has to be run.
 */
gushort get_server_port(GKeyFile *settings) {
    GError *error = NULL;

    gushort server_port
        = g_key_file_get_integer(settings, SERVER_GROUP, SERVER_PORT, &error);

    if (server_port != 0) {
        if ((server_port >= MIN_PORT) && (server_port <= MAX_PORT)) {
            return server_port;
        } else {
            g_warning(ERR_PORT_VALID_MUST_BE_POSITIVE_INT); return DEF_PORT;
        }
    } else {
        g_warning(ERR_PORT_VALID_MUST_BE_POSITIVE_INT); return DEF_PORT;
    }
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
    else {
        g_free(datastore  );
        g_free(path_dir   );
        g_free(path_prefix);

        return NULL;
    }

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

// Helper function. Makes final pointers cleanups/unrefs, closes streams, etc.
void _cleanup(_CLEANUP_ARGS *cleanup_args) {
    g_message(       MSG_SERVER_STOPPED);
    syslog(LOG_INFO, MSG_SERVER_STOPPED);

    // Closing the system logger.
    closelog();

    g_output_stream_close((GOutputStream *)
                   cleanup_args->log_stream, NULL, NULL);
    g_object_unref(cleanup_args->log_stream);
    g_object_unref(cleanup_args->logfile);

    g_main_loop_quit(cleanup_args->loop);
}

// vim:set nu et ts=4 sw=4:
