/*
 * src/busd.h
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.2.0
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

#include <stdio.h>
#include <syslog.h>

#define G_LOG_USE_STRUCTURED // <== To use structured logging.

#include <libsoup/soup.h>
#include <glib-unix.h> // <== Needs this for importing `g_unix_signal_add()`.
#include <json-glib/json-glib.h>

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
#define ERR_CANNOT_START_SERVER "FATAL: Cannot start server "
#define ERR_SERV_UNKNOWN_REASON "for an unknown reason. Quitting..."
#define ERR_ADDR_ALREADY_IN_USE "due to address requested already in use. " \
    "Quitting..."
#define ERR_REQ_PARAMS_MUST_BE_POSITIVE_INTS "Request parameters must take " \
    "positive integer values, in the range 1 .. 2,147,483,647. " \
    "Please check your inputs."
#define ERR_EADDRINUSE_CODE 33

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

#define LOG_LEVEL_WARN  "WARN"
#define LOG_LEVEL_DEBUG "DEBUG"
#define LOG_LEVEL_INFO  "INFO"

#define DTM_FORMAT "%02u"
#define LOG_FORMAT "%s"

// Allowed HTTP methods.
#define HTTP_HEAD "HEAD"
#define HTTP_GET  "GET"

// REST URI path-related constants.
#define REST_PREFIX "route"
#define REST_DIRECT "direct"

// HTTP response-related constants.
#define MIME_TYPE                "application/json"
#define HDR_SERVER_P             "server-header"
#define HDR_ALLOW_N              "Allow"
#define HDR_ALLOW_V              "GET, HEAD"
#define ERROR_JSON_KEY           "error"
#define ERROR_JSON_VAL_NOT_FOUND "404 Not Found."

// HTTP request parameter names.
#define FROM "from"
#define TO   "to"

/**
 * The regex pattern for the element to be excluded from a bus stops sequence:
 * it is an arbitrary identifier of a route, which is not used
 * in the routes processing anyhow.
 */
#define ROUTE_ID_REGEX "^\\d+"

// The log writer callback. Gets called on every message logging attempt.
GLogWriterOutput log_writer(      GLogLevelFlags,
                            const GLogField *,
                                  gsize,
                                  gpointer);

// Retrieves the port number used to run the server, from daemon settings.
gushort get_server_port(GKeyFile *);

// Identifies whether debug logging is enabled by retrieving
// the corresponding setting from daemon settings.
gboolean is_debug_log_enabled(GKeyFile *);

// Retrieves the path and filename of the routes data store
// from daemon settings.
gchar *get_routes_datastore(GKeyFile *);

// Helper structure to hold args for the `_cleanup()` helper function.
typedef struct {
    GFileOutputStream *log_stream;
    GFile             *logfile;
    GMainLoop         *loop;
} _CLEANUP_ARGS;

// Starts up the Soup web server and the main loop.
GMainLoop *startup(const gushort,
                   const gboolean,
                         GPtrArray *,
                         _CLEANUP_ARGS *);

// The structure to hold request handler payload data
// to pass to the default request handler callback.
typedef struct {
    gboolean   debug_log_enabled;
    GPtrArray *routes_list;
} HANDLER_PAYLOAD;

// The default request handler callback. Used to process the incoming request.
void request_handler(      SoupServer *,
                           SoupServerMessage *,
                     const char *,
                           GHashTable *,
                           gpointer);

// Helper protos.
GKeyFile *_get_settings();
void _cleanup(_CLEANUP_ARGS *);

#endif//BUSD_H

// vim:set nu et ts=4 sw=4:
