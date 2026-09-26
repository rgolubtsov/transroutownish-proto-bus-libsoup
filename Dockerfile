#
# Dockerfile
# =============================================================================
# Urban bus routing microservice prototype (C port). Version 0.3.1
# =============================================================================
# A daemon written in C (GNOME/libsoup), designed and intended to be run
# as a microservice, implementing a simple urban bus routing prototype.
# =============================================================================
# Copyright (C) 2023-2026 Radislav (Radicchio) Golubtsov
#
# (See the LICENSE file at the top of the source tree.)
#

# === Stage 1: Install dependencies ===========================================
FROM       alpine:latest
RUN        ["apk", "add", "make"         ]
RUN        ["apk", "add", "gcc"          ]
RUN        ["apk", "add", "musl-dev"     ]
RUN        ["apk", "add", "libsoup3-dev" ]
RUN        ["apk", "add", "json-glib-dev"]

# === Stage 2: Build the microservice =========================================
USER       daemon
WORKDIR    var/tmp
COPY       src      bus/src/
COPY       etc      bus/etc/
COPY       data     bus/data/
COPY       Makefile bus/
WORKDIR    bus
USER       root
RUN        ["chown", "-R", "daemon:daemon", "."]
USER       daemon
RUN        ["make", "clean"]
RUN        ["make", "all"  ]

# === Stage 3: Run the microservice ===========================================
ENTRYPOINT ["bin/busd"]

# vim:set nu ts=4 sw=4:
