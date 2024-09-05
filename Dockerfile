#
# Dockerfile
# =============================================================================
# Urban bus routing microservice prototype (C port). Version 0.3.0
# =============================================================================
# A daemon written in C (GNOME/libsoup), designed and intended to be run
# as a microservice, implementing a simple urban bus routing prototype.
# =============================================================================
# Copyright (C) 2023-2024 Radislav (Radicchio) Golubtsov
#
# (See the LICENSE file at the top of the source tree.)
#

# === Stage 1: Build the microservice =========================================
FROM       alpine:latest AS build
USER       daemon
WORKDIR    var/tmp
RUN        ["mkdir", "-p", "bus/src", "bus/etc", "bus/data"]
COPY       src  bus/src/
COPY       etc  bus/etc/
COPY       data bus/data/
WORKDIR    bus
RUN        ["make", "clean"]
RUN        ["make", "all"  ]

# === Stage 2: Run the microservice ===========================================
ENTRYPOINT ["bus/bin/busd"]

# vim:set nu ts=4 sw=4:
