/*
 * src/bus-handler.c
 * ============================================================================
 * Urban bus routing microservice prototype (C port). Version 0.3.1
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

    const char *uri = g_uri_to_string(soup_server_message_get_uri(msg));

    const char *method = soup_server_message_get_method(msg);
    SoupMessageHeaders *resp_headers
        = soup_server_message_get_response_headers(msg);

    if ((g_strcmp0(   method, HTTP_HEAD) != 0)
        && (g_strcmp0(method, HTTP_GET ) != 0)) {

        soup_message_headers_append(resp_headers, HDR_ALLOW_N, HDR_ALLOW_V);

        soup_server_message_set_status(msg,
            SOUP_STATUS_METHOD_NOT_ALLOWED, NULL);

        return;
    }

    JsonObject    *json_object = json_object_new();
    JsonNode      *json_node   = json_node_new(JSON_NODE_OBJECT);
    JsonGenerator *json_gen    = json_generator_new();
    GString       *json_body   = g_string_new(NULL);

    // GET /route/direct
    if (g_strcmp0(path, SLASH REST_PREFIX SLASH REST_DIRECT) != 0) {
        g_debug(LOG_FORMAT, uri);

        soup_server_message_set_status(msg, SOUP_STATUS_NOT_FOUND, NULL);

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

    gchar *from_ = EMPTY_STRING;
    gchar *to_   = EMPTY_STRING;

    if (query != NULL) {
        from_ = g_hash_table_lookup(query, FROM);
        to_   = g_hash_table_lookup(query, TO  );
    }

    HANDLER_PAYLOAD *handler_payload = payload;
    gboolean debug_log_enabled = handler_payload->debug_log_enabled;

    if (debug_log_enabled) {
g_debug(         FROM EQUALS LOG_FORMAT SPACE V_BAR SPACE TO EQUALS LOG_FORMAT,
                 from_,                                   to_);
syslog(LOG_DEBUG,FROM EQUALS LOG_FORMAT SPACE V_BAR SPACE TO EQUALS LOG_FORMAT,
                 from_,                                   to_);
    }

    // ------------------------------------------------------------------------
    // --- Parsing and validating request params - Begin ----------------------
    // ------------------------------------------------------------------------
    gboolean is_request_malformed = FALSE;

    unsigned from = g_ascii_strtoull(from_, NULL, 10);
    unsigned to   = g_ascii_strtoull(to_,   NULL, 10);

    if ((from < 1) || (to < 1)) {
        is_request_malformed = TRUE;
    }
    // ------------------------------------------------------------------------
    // --- Parsing and validating request params - End ------------------------
    // ------------------------------------------------------------------------

    if (is_request_malformed) {
        g_debug(LOG_FORMAT, uri);

        soup_server_message_set_status(msg, SOUP_STATUS_BAD_REQUEST, NULL);

        json_object_set_string_member(json_object, ERROR_JSON_KEY,
            ERR_REQ_PARAMS_MUST_BE_POSITIVE_INTS);
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

    GPtrArray *routes_list = handler_payload->routes_list;

    int len_from_ = snprintf(NULL, 0, INT_FORMAT, from);
    int len_to_   = snprintf(NULL, 0, INT_FORMAT, to  );
            from_ = malloc(len_from_ + 1);
            to_   = malloc(len_to_   + 1);
    snprintf(from_, (len_from_ + 1),  INT_FORMAT, from);
    snprintf(to_,   (len_to_   + 1),  INT_FORMAT, to  );

    // Performing the routes processing to find out the direct route.
    gboolean direct = find_direct_route(
        debug_log_enabled,
        routes_list,
        from_,
        to_);

    free(to_  );
    free(from_);

    soup_server_message_set_status(msg, SOUP_STATUS_OK, NULL);

    json_object_set_int_member(    json_object, FROM,        from  );
    json_object_set_int_member(    json_object, TO,          to    );
    json_object_set_boolean_member(json_object, REST_DIRECT, direct);
    json_node = json_node_init_object(json_node, json_object);
    json_generator_set_root(json_gen, json_node);
    json_body = json_generator_to_gstring(json_gen, json_body);

    soup_server_message_set_response(msg, MIME_TYPE, SOUP_MEMORY_COPY,
        json_body->str, json_body->len);

    g_string_free(json_body, TRUE);
    json_node_free(json_node);
    json_object_unref(json_object);
}

/**
 * Performs the routes processing (onto bus stops sequences) to identify
 * and return whether a particular interval between two bus stop points
 * given is direct (i.e. contains in any of the routes), or not.
 *
 * @param debug_log_enabled The debug logging enabler.
 * @param routes_list       A list containing all available routes.
 * @param from              The starting bus stop point.
 * @param to                The ending   bus stop point.
 *
 * @return <code>TRUE</code> if the direct route is found,
 *         <code>FALSE</code> otherwise.
 */
gboolean find_direct_route(const gboolean   debug_log_enabled,
                           const GPtrArray *routes_list,
                           const gchar     *from,
                           const gchar     *to) {

//  guint routes_len = routes_list->len;

//  for (guint i = 0; i < routes_len; i++) {
//      g_debug("%u" SPACE EQUALS SPACE LOG_FORMAT, (i + 1),
//          (gchar *) g_ptr_array_index(routes_list, i));
//  }

    gboolean direct = FALSE;

    // Two bus stop points in a route cannot point up to the same value.
    if (g_strcmp0(from, to) == 0) { return direct; }

    gchar *route = EMPTY_STRING, *route_from = EMPTY_STRING;

    guint routes_count = routes_list->len;

    for (guint i = 0; i < routes_count; i++) {
        route = g_ptr_array_index(routes_list, i);

        if (debug_log_enabled) {
            g_debug(INT_FORMAT SPACE EQUALS SPACE LOG_FORMAT, (i + 1), route);
        }

        if (g_regex_match_simple(g_strconcat(SEQ1_REGEX, from, SEQ2_REGEX,
            NULL), route, 0, 0)) {

            // Pinning in the starting bus stop point, if it's found.
            // Next, searching for the ending bus stop point
            // on the current route, beginning at the pinned point.
            route_from = g_strrstr(route, from);

            if (debug_log_enabled) {
                g_debug(LOG_FORMAT SPACE V_BAR SPACE LOG_FORMAT,
                    from, route_from);
            }

            if (g_regex_match_simple(g_strconcat(SEQ1_REGEX, to, SEQ2_REGEX,
                NULL), route_from, 0, 0)) {

                direct = TRUE; break;
            }
        }
    }

    return direct;
}

// vim:set nu et ts=4 sw=4:
