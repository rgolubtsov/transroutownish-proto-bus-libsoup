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

#include <glib.h>

// Helper constants.
#define EMPTY_STRING ""
#define SPACE       " "
#define SLASH       "/"
#define EQUALS      "="
#define V_BAR       "|"
#define NEW_LINE   "\n"

// Common error messages.
#define ERR_SETTINGS_NOT_FOUND "Daemon settings file not found: %s"

// Common notification messages.
#define MSG_SERVER_STARTED "%s: Server started on port %u"
#define MSG_SERVER_STOPPED "%s: Server stopped"

/** The default server port number. */
#define DEF_PORT 8080

/** The path and filename of the daemon settings. */
#define SETTINGS "./etc/settings.conf"

// Daemon settings keys for the server port number.
#define SERVER_GROUP "Server"
#define SERVER_PORT  "port"

// Daemon settings keys for the logger.
#define LOGGER_GROUP "Logger"
#define LOG_ENABLED  "debug.enabled"

// Helper protos.
GKeyFile *_get_settings();

gboolean is_debug_log_enabled(GKeyFile *);
unsigned short get_server_port(GKeyFile *);

#endif//BUSD_H

// vim:set nu et ts=4 sw=4:
