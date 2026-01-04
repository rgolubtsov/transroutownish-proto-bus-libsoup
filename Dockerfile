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
# Note: Need to use Alpine `edge` image instead of `latest` due to utilizing
#       the Tiny C Compiler (TCC) at linking pass. TCC is currently available
#       in the [testing] repository of the `edge` branch only.
FROM       alpine:edge
RUN        ["apk", "add", "make"           ] #         +----------+
RUN        ["apk", "add", "gcc"            ] #         |  Alpine  |
RUN        ["apk", "add", "pkgconf"        ] #         |  Linux   |
RUN        ["apk", "add", "musl-dev"       ] #         +----------+
RUN        ["apk", "add", "libsoup3-dev"   ] #         |          |
RUN        ["apk", "add", "json-glib-dev"  ] #         V          V
RUN        echo 'https://dl-cdn.alpinelinux.org/alpine/edge/testing' >> /etc/apk/repositories
RUN        ["apk", "add", "tcc"            ]
RUN        ["apk", "add", "tcc-libs-static"]

# === Stage 2: Build the microservice =========================================
USER       daemon
WORKDIR    var/tmp
RUN        ["mkdir", "-p", "bus/src", "bus/etc", "bus/data"]
COPY       src      bus/src/
COPY       etc      bus/etc/
COPY       data     bus/data/
COPY       Makefile bus/
WORKDIR    bus
RUN        ["make", "clean"]
RUN        ["make", "all"  ]

# === Stage 3: Run the microservice ===========================================
ENTRYPOINT ["bin/busd"]

# vim:set nu ts=4 sw=4:
