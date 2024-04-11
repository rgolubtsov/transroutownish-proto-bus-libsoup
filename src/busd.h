/*
 * src/busd.h
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

#ifndef BUSD_H
#define BUSD_H

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#define G_LOG_USE_STRUCTURED // <== To use structured logging.

#include <glib.h>
#include <gio/gio.h>

// Helper constants.
#define EMPTY_STRING ""
#define SPACE       " "
#define SLASH       "/"
#define EQUALS      "="
#define V_BAR       "|"
#define NEW_LINE   "\n"

// Common error messages.
#define ERR_PORT_VALID_MUST_BE_POSITIVE_INT "Valid server port must be " \
    "a positive integer value, in the range 1024 .. 49151. " \
    "The default value of 8080 will be used instead."
#define ERR_SETTINGS_NOT_FOUND "Daemon settings file not found: %s"
#define ERR_DATASTORE_NOT_FOUND "FATAL: Data store file not found. Quitting..."

// Common notification messages.
#define MSG_SERVER_STARTED "Server started on port %u"
#define MSG_SERVER_STOPPED "Server stopped"

/** The path and filename of the daemon settings. */
#define SETTINGS "./etc/settings.conf"

/** The path and filename of the sample routes data store. */
#define SAMPLE_ROUTES "./data/routes.txt"

/** The minimum port number allowed. */
#define MIN_PORT 1024

/** The maximum port number allowed. */
#define MAX_PORT 49151

/** The default server port number. */
#define DEF_PORT 8080

// Daemon settings keys for the server port number.
#define SERVER_GROUP "Server"
#define SERVER_PORT  "port"

// Daemon settings keys for the logger.
#define LOGGER_GROUP "Logger"
#define LOG_ENABLED  "debug.enabled"

// Daemon settings keys for the routes data store.
#define ROUTES_GROUP "Routes"
#define PATH_PREFIX  "datastore.path.prefix"
#define PATH_DIR     "datastore.path.dir"
#define FILENAME     "datastore.filename"

#define LOG_DIR "./log/"
#define LOGFILE "bus.log"

#define LOG_KEY_MESSAGE "MESSAGE"

// The log writer callback. Gets called on every message logging attempt.
GLogWriterOutput log_writer(      GLogLevelFlags,
                            const GLogField *,
                                  gsize,
                                  gpointer);

// Retrieves the port number used to run the server, from daemon settings.
unsigned short get_server_port(GKeyFile *);

// Identifies whether debug logging is enabled by retrieving
// the corresponding setting from daemon settings.
gboolean is_debug_log_enabled(GKeyFile *);

// Retrieves the path and filename of the routes data store
// from daemon settings.
gchar *get_routes_datastore(GKeyFile *);

// Helper protos.
GKeyFile *_get_settings();
void _cleanup(GFileOutputStream *, GFile *);

#endif//BUSD_H

// vim:set nu et ts=4 sw=4:
