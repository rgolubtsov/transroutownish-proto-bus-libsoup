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

// Helper function. Used to get the daemon settings.
GKeyFile *_get_settings() {
    GKeyFile *settings = g_key_file_new();
    GError   *error    = NULL;

    gboolean is_loaded = g_key_file_load_from_file(settings,
        "./etc/settings.conf", G_KEY_FILE_NONE, &error);

    if (!is_loaded) {
        gboolean is_failed = g_error_matches(error, G_FILE_ERROR,
            G_FILE_ERROR_NOENT);

        if (is_failed) {
            g_warning(ERR_SETTINGS_NOT_FOUND, error->message);
        }

        return NULL;
    }

    return settings;
}

// vim:set nu et ts=4 sw=4:
