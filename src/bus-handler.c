/*
 * src/bus-handler.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.1.0
 * ============================================================================
 * A daemon written in C (GNOME/libsoup), designed and intended to be run
 * as a microservice, implementing a simple urban bus routing prototype.
 * ============================================================================
 * Copyright (C) 2023-2024 Radislav (Radicchio) Golubtsov
 *
 * (See the LICENSE file at the top of the source tree.)
 */

// The request handler module of the daemon -----------------------------------

#include "busd.h"

/**
 * The default request handler callback.
 * Used to process the incoming request.
 *
 * @param server  The Soup web server instance.
 * @param msg     The request message to be processed.
 * @param path    The path  component of request message URI.
 * @param query   The query component of request message URI.
 * @param payload The pointer to a payload data passed from the controller.
 */
void request_handler(      SoupServer        *server,
                           SoupServerMessage *msg,
                     const char              *path,
                           GHashTable        *query,
                           gpointer           payload) {

    const char *method = soup_server_message_get_method(msg);

    g_debug("[%s][%s]", method, path);

    if ((g_strcmp0(   method, HTTP_HEAD) != 0)
        && (g_strcmp0(method, HTTP_GET ) != 0)) {

        soup_server_message_set_status(msg,
            SOUP_STATUS_METHOD_NOT_ALLOWED, NULL);

        return;
    }

    if (g_strcmp0(path, SLASH REST_PREFIX SLASH REST_DIRECT) != 0) {
        g_debug("[%s]", path);

        soup_server_message_set_status(msg, SOUP_STATUS_BAD_REQUEST, NULL);

        return;
    }

    soup_server_message_set_status(msg, SOUP_STATUS_NO_CONTENT, NULL);
}

// vim:set nu et ts=4 sw=4:
