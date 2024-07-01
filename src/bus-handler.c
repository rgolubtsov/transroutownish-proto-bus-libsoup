/*
 * src/bus-handler.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.0.3
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

    soup_server_message_set_status(msg, SOUP_STATUS_NO_CONTENT, NULL);
}

// vim:set nu et ts=4 sw=4:
