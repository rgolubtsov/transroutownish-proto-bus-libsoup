/*
 * src/bus-helper.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.0.1
 * ============================================================================
 * A daemon written in C (GNOME/libsoup), designed and intended to be run
 * as a microservice, implementing a simple urban bus routing prototype.
 * ============================================================================
 * Copyright (C) 2023 Radislav (Radicchio) Golubtsov
 *
 * (See the LICENSE file at the top of the source tree.)
 */

// The helper module for the daemon -------------------------------------------

#include "busd.h"

// Helper function. Used to get the daemon settings.
GKeyFile *_get_settings() {
    GKeyFile *settings = g_key_file_new();

    g_key_file_load_from_file(settings,
        "./etc/settings.conf", G_KEY_FILE_NONE, NULL);

    return settings;
}

// vim:set nu et ts=4 sw=4:
