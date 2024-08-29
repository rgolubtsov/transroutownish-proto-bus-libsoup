/*
 * src/bus-handler.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.1.3
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
    SoupMessageHeaders *resp_headers
        = soup_server_message_get_response_headers(msg);

    g_debug("[%s][%s]", method, path);

    if ((g_strcmp0(   method, HTTP_HEAD) != 0)
        && (g_strcmp0(method, HTTP_GET ) != 0)) {

        soup_message_headers_append(resp_headers, HDR_ALLOW_N, HDR_ALLOW_V);

        soup_server_message_set_status(msg,
            SOUP_STATUS_METHOD_NOT_ALLOWED, NULL);

        return;
    }

    if (g_strcmp0(path, SLASH REST_PREFIX SLASH REST_DIRECT) != 0) {
        g_debug("[%s]", path);

        soup_server_message_set_status(msg, SOUP_STATUS_NOT_FOUND, NULL);

        JsonObject    *json_object = json_object_new();
        JsonNode      *json_node   = json_node_new(JSON_NODE_OBJECT);
        JsonGenerator *json_gen    = json_generator_new();
        GString       *json_body   = g_string_new(NULL);

        json_object_set_string_member(json_object, ERROR_JSON_KEY,
            ERROR_JSON_VAL_NOT_FOUND);
        json_node = json_node_init_object(json_node, json_object);
        json_generator_set_root(json_gen, json_node);
        json_body = json_generator_to_gstring(json_gen, json_body);

        soup_server_message_set_response(msg, MIME_TYPE, SOUP_MEMORY_COPY,
            json_body->str, json_body->len);

        g_string_free(json_body, TRUE);
        json_node_free(json_node);
        json_object_unref(json_object);

        return;
    }

    gchar *from_ = g_hash_table_lookup(query, FROM);
    gchar *to_   = g_hash_table_lookup(query, TO  );

    g_debug(          FROM EQUALS "%s" SPACE V_BAR SPACE TO EQUALS "%s",
                      from_,                             to_);
    syslog(LOG_DEBUG, FROM EQUALS "%s" SPACE V_BAR SPACE TO EQUALS "%s",
                      from_,                             to_);

    // ------------------------------------------------------------------------
    // --- Parsing and validating request params - Begin ----------------------
    // ------------------------------------------------------------------------
    gboolean is_request_malformed __attribute__ ((unused)) = FALSE;

    gchar *from __attribute__ ((unused)) = EMPTY_STRING;
    gchar *to   __attribute__ ((unused)) = EMPTY_STRING;

    // TODO: Implement parsing and validating request params mechs.

    // ------------------------------------------------------------------------
    // --- Parsing and validating request params - End ------------------------
    // ------------------------------------------------------------------------

    soup_server_message_set_status(msg, SOUP_STATUS_NO_CONTENT, NULL);
}

// vim:set nu et ts=4 sw=4:
