# Trans-RoutE-Townish (transroutownish) :small_orange_diamond: Urban bus routing microservice prototype (C port)

**A daemon written in C (GNOME/libsoup), designed and intended to be run as a microservice,
<br />implementing a simple urban bus routing prototype**

**Rationale:** This project is a *direct* **[C99](https://en.cppreference.com/w/c/99 "The C Programming Language, ISO/IEC 9899:1999 standard, a.k.a. C99")** port of the earlier developed **urban bus routing prototype**, written in Groovy using **[Ratpack](https://ratpack.io "A set of Java libraries for building scalable HTTP applications")** Java libraries, and tailored to be run as a microservice in a Docker container. The following description of the underlying architecture and logics has been taken **[from here](https://github.com/rgolubtsov/transroutownish-proto-bus-groovy)** as is, without any modifications or adjustment.

Consider an IoT system that aimed at planning and forming a specific bus route for a hypothetical passenger. One crucial part of such system is a **module**, that is responsible for filtering bus routes between two arbitrary bus stops where a direct route is actually present and can be easily found. Imagine there is a fictional urban public transportation agency that provides a wide series of bus routes, which covered large city areas, such that they are consisting of many bus stop points in each route. Let's name this agency **Trans-RoutE-Townish Co., Ltd.** or in the Net representation &mdash; **transroutownish.com**, hence the name of the project.

A **module** that is developed here is dedicated to find out quickly, whether there is a direct route in a list of given bus routes between two specified bus stops. It should immediately report back to the IoT system with the result `true` if such a route is found, i.e. it exists in the bus routes list, or `false` otherwise, by outputting a simple JSON structure using the following format:

```
{
    "from"   : <starting_bus_stop_point>,
    "to"     : <ending_bus_stop_point>,
    "direct" : true
}
```

`<starting_bus_stop_point>` and `<ending_bus_stop_point>` above are bus stop IDs: unique positive integers, taken right from inputs.

A bus routes list is a plain text file where each route has its own unique ID (positive integer) and a sequence of its bus stop IDs. Each route occupies only one line in this file, so that they are all representing something similar to a list &mdash; the list of routes. The first number in a route is always its own ID. Other consequent numbers after it are simply IDs of bus stops in this route, up to the end of line. All IDs in each route are separated by whitespace, usually by single spaces or tabs, but not newline.

There are some constraints:
1. Routes are considered not to be a round trip journey, that is they are operated in the forward direction only.
2. All IDs (of routes and bus stops) must be represented by positive integer values, in the range `1 .. 2,147,483,647`.
3. Any bus stop ID may occure in the current route only once, but it might be presented in any other route too.

The list of routes is usually mentioned throughout the source code as a **routes data store**, and a sample routes data store can be found in the `data/` directory of this repo.

Since the microservice architecture for building independent backend modules of a composite system are very prevalent nowadays, this seems to be natural for creating a microservice, which is containerized and run as a daemon, serving a continuous flow of HTTP requests.

This microservice is intended to be built locally and to be run like a conventional daemon in the VM environment, as well as a containerized service, managed by Docker.

One may consider this project has to be suitable for a wide variety of applied areas and may use this prototype as: (1) a template for building a similar microservice, (2) for evolving it to make something more universal, or (3) to simply explore it and take out some snippets and techniques from it for *educational purposes*, etc.

---

## Table of Contents

* **[Building](#building)**
  * **[Creating a Docker image](#creating-a-docker-image)**
* **[Running](#running)**
  * **[Running a Docker image](#running-a-docker-image)**
  * **[Exploring a Docker image payload](#exploring-a-docker-image-payload)**
* **[Consuming](#consuming)**
  * **[Logging](#logging)**
  * **[Error handling](#error-handling)**

## Building

The microservice might be built and run successfully under **Ubuntu Server (Ubuntu 22.04.4 LTS x86-64)** and **Arch Linux** (both proven). &mdash; First install the necessary dependencies (`build-essential`, `libsoup-3.0-dev`, `libjson-glib-dev`, `docker-buildx`):

* In Ubuntu Server:

```
$ sudo apt-get update && \
  sudo apt-get install build-essential libsoup-3.0-dev libjson-glib-dev docker-buildx -y
...
```

* In Arch Linux:

```
$ sudo pacman -Syu base-devel libsoup3 json-glib docker docker-buildx
...
```

**Build** the microservice using **GNU Make**:

```
$ make clean
rm -f -vR bin src/bus-core.o src/bus-controller.o src/bus-handler.o src/bus-helper.o
$
$ make all  # <== Building the daemon.
cc -Wall -std=c99 -O3 -c `pkg-config --cflags-only-I libsoup-3.0 json-glib-1.0` src/bus-core.c -o src/bus-core.o
cc -Wall -std=c99 -O3 -c `pkg-config --cflags-only-I libsoup-3.0 json-glib-1.0` src/bus-controller.c -o src/bus-controller.o
cc -Wall -std=c99 -O3 -c `pkg-config --cflags-only-I libsoup-3.0 json-glib-1.0` src/bus-handler.c -o src/bus-handler.o
cc -Wall -std=c99 -O3 -c `pkg-config --cflags-only-I libsoup-3.0 json-glib-1.0` src/bus-helper.c -o src/bus-helper.o
if [ ! -d bin ]; then \
    mkdir bin; \
fi
cc -s -o bin/busd src/bus-core.o src/bus-controller.o src/bus-handler.o src/bus-helper.o `pkg-config   --libs-only-l libsoup-3.0 json-glib-1.0`
```

### Creating a Docker image

**Build** a Docker image for the microservice:

```
$ # Pull the Alpine Linux image first, if not already there:
$ sudo docker pull alpine:latest
...
$ # Then build the microservice image:
$ sudo docker build -ttransroutownish/busc99 .
...
```

## Running

**Run** the microservice using its executable directly, built previously by the `all` target:

```
$ ./bin/busd; echo $?
...
```

To run the microservice as a *true* daemon, i.e. in the background, redirecting all the console output to `/dev/null`, the following form of invocation of its executable can be used:

```
$ ./bin/busd > /dev/null 2>&1 &
[1] <pid>
```

**Note:** This will suppress all the console output only; logging to a logfile and to the Unix syslog will remain unchanged.

The daemonized microservice then can be stopped gracefully at any time by issuing the following command:

```
$ kill -SIGTERM <pid>
$
[1]+  Done                       ./bin/busd > /dev/null 2>&1
```

### Running a Docker image

**Run** a Docker image of the microservice, deleting all stopped containers prior to that:

```
$ sudo docker rm `sudo docker ps -aq`; \
  export PORT=8765 && sudo docker run -dp${PORT}:${PORT} --name busc99 transroutownish/busc99; echo $?
...
```

### Exploring a Docker image payload

The following is not necessary but might be considered somewhat interesting &mdash; to look into the running container and check out that the microservice's daemon executable, config, logfile, and routes data store are at their expected places and in effect:

```
$ sudo docker ps -a
CONTAINER ID   IMAGE                    COMMAND      CREATED              STATUS              PORTS                                         NAMES
<container_id> transroutownish/busc99   "bin/busd"   About a minute ago   Up About a minute   0.0.0.0:8765->8765/tcp, [::]:8765->8765/tcp   busc99
$
$ sudo docker exec -it busc99 sh; echo $?
/var/tmp/bus $
/var/tmp/bus $ uname -a
Linux <container_id> 7.0.0-31-generic #31-Ubuntu SMP PREEMPT_DYNAMIC Sat Aug  1 04:26:38 UTC 2026 x86_64 Linux
/var/tmp/bus $
/var/tmp/bus $ cat /etc/os-release /etc/alpine-release
NAME="Alpine Linux"
ID=alpine
VERSION_ID=3.24.2
PRETTY_NAME="Alpine Linux v3.24"
HOME_URL="https://alpinelinux.org/"
BUG_REPORT_URL="https://gitlab.alpinelinux.org/alpine/aports/-/issues"
3.24.2
/var/tmp/bus $
/var/tmp/bus $ cc --version
cc (Alpine 15.2.0) 15.2.0
Copyright (C) 2025 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

/var/tmp/bus $
/var/tmp/bus $ ls -al
total 44
drwxr-xr-x    1 daemon   daemon        4096 Sep 28 17:50 .
drwxrwxrwt    1 root     root          4096 Sep 28 17:10 ..
-rw-rw-r--    1 daemon   daemon        1428 Sep 26 22:50 Makefile
drwxr-xr-x    2 daemon   daemon        4096 Sep 28 17:10 bin
drwxr-xr-x    1 daemon   daemon        4096 Apr 11  2024 data
drwxr-xr-x    1 daemon   daemon        4096 Sep 28 17:10 etc
drwxr-xr-x    2 daemon   daemon        4096 Sep 28 17:50 log
drwxr-xr-x    1 daemon   daemon        4096 Sep 28 17:10 src
/var/tmp/bus $
/var/tmp/bus $ ls -al bin/ data/ etc/ log/ src/
bin/:
total 32
drwxr-xr-x    2 daemon   daemon        4096 Sep 28 17:10 .
drwxr-xr-x    1 daemon   daemon        4096 Sep 28 17:50 ..
-rwxr-xr-x    1 daemon   daemon       22496 Sep 28 17:10 busd

data/:
total 60
drwxr-xr-x    1 daemon   daemon        4096 Apr 11  2024 .
drwxr-xr-x    1 daemon   daemon        4096 Sep 28 17:50 ..
-rw-rw-r--    1 daemon   daemon       46218 Nov 13  2023 routes.txt

etc/:
total 16
drwxr-xr-x    1 daemon   daemon        4096 Sep 28 17:10 .
drwxr-xr-x    1 daemon   daemon        4096 Sep 28 17:50 ..
-rw-rw-r--    1 daemon   daemon         808 Sep 28 17:00 settings.conf

log/:
total 12
drwxr-xr-x    2 daemon   daemon        4096 Sep 28 17:50 .
drwxr-xr-x    1 daemon   daemon        4096 Sep 28 17:50 ..
-rw-r--r--    1 daemon   daemon          59 Sep 28 17:50 bus.log

src/:
total 84
drwxr-xr-x    1 daemon   daemon        4096 Sep 28 17:10 .
drwxr-xr-x    1 daemon   daemon        4096 Sep 28 17:50 ..
-rw-rw-r--    1 daemon   daemon        3311 Sep 26 22:50 bus-controller.c
-rw-r--r--    1 daemon   daemon        4088 Sep 28 17:10 bus-controller.o
-rw-rw-r--    1 daemon   daemon        4836 Sep 26 22:50 bus-core.c
-rw-r--r--    1 daemon   daemon        5944 Sep 28 17:10 bus-core.o
-rw-rw-r--    1 daemon   daemon        8452 Sep 26 22:50 bus-handler.c
-rw-r--r--    1 daemon   daemon        8712 Sep 28 17:10 bus-handler.o
-rw-rw-r--    1 daemon   daemon        7593 Sep 26 22:50 bus-helper.c
-rw-r--r--    1 daemon   daemon       10376 Sep 28 17:10 bus-helper.o
-rw-rw-r--    1 daemon   daemon        6075 Sep 26 22:50 busd.h
/var/tmp/bus $
/var/tmp/bus $ ldd bin/busd
        /lib/ld-musl-x86_64.so.1 (0x7eac8adf7000)
        libsoup-3.0.so.0 => /usr/lib/libsoup-3.0.so.0 (0x7eac8ad69000)
        libjson-glib-1.0.so.0 => /usr/lib/libjson-glib-1.0.so.0 (0x7eac8ad45000)
        libgio-2.0.so.0 => /usr/lib/libgio-2.0.so.0 (0x7eac8ab55000)
        libgobject-2.0.so.0 => /usr/lib/libgobject-2.0.so.0 (0x7eac8aaf4000)
        libglib-2.0.so.0 => /usr/lib/libglib-2.0.so.0 (0x7eac8a997000)
        libc.musl-x86_64.so.1 => /lib/ld-musl-x86_64.so.1 (0x7eac8adf7000)
        libintl.so.8 => /usr/lib/libintl.so.8 (0x7eac8a973000)
        libsqlite3.so.0 => /usr/lib/libsqlite3.so.0 (0x7eac8a7e2000)
        libpsl.so.5 => /usr/lib/libpsl.so.5 (0x7eac8a7ce000)
        libbrotlidec.so.1 => /usr/lib/libbrotlidec.so.1 (0x7eac8a7bf000)
        libz.so.1 => /usr/lib/libz.so.1 (0x7eac8a7a4000)
        libnghttp2.so.14 => /usr/lib/libnghttp2.so.14 (0x7eac8a782000)
        libgmodule-2.0.so.0 => /usr/lib/libgmodule-2.0.so.0 (0x7eac8a77b000)
        libmount.so.1 => /usr/lib/libmount.so.1 (0x7eac8a733000)
        libffi.so.8 => /usr/lib/libffi.so.8 (0x7eac8a729000)
        libpcre2-8.so.0 => /usr/lib/libpcre2-8.so.0 (0x7eac8a668000)
        libidn2.so.0 => /usr/lib/libidn2.so.0 (0x7eac8a636000)
        libunistring.so.5 => /usr/lib/libunistring.so.5 (0x7eac8a45f000)
        libbrotlicommon.so.1 => /usr/lib/libbrotlicommon.so.1 (0x7eac8a43c000)
        libblkid.so.1 => /usr/lib/libblkid.so.1 (0x7eac8a409000)
        libeconf.so.0 => /usr/lib/libeconf.so.0 (0x7eac8a3fe000)
/var/tmp/bus $
/var/tmp/bus $ netstat -plunt
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name
tcp        0      0 0.0.0.0:8765            0.0.0.0:*               LISTEN      1/busd
tcp        0      0 :::8765                 :::*                    LISTEN      1/busd
/var/tmp/bus $
/var/tmp/bus $ ps aux
PID   USER     TIME  COMMAND
    1 daemon    0:00 bin/busd
   10 daemon    0:00 sh
   23 daemon    0:00 ps aux
/var/tmp/bus $
/var/tmp/bus $ exit # Or simply <Ctrl-D>.
0
```

## Consuming

All the routes are contained in a so-called **routes data store**. It is located in the `data/` directory. The default filename for it is `routes.txt`, but it can be specified explicitly (if intended to use another one) in the `etc/settings.conf` configuration file.

**Identify**, whether there is a direct route between two bus stops with IDs given in the **HTTP GET** request, searching for them against the underlying **routes data store**:

HTTP request param | Sample value | Another sample value | Yet another sample value
------------------ | ------------ | -------------------- | ------------------------
`from`             | `4838`       | `82`                 | `2147483647`
`to`               | `524987`     | `35390`              | `1`

The direct route is found:

```
$ curl 'http://localhost:8765/route/direct?from=4838&to=524987'
{"from":4838,"to":524987,"direct":true}
```

The direct route is not found:

```
$ curl 'http://localhost:8765/route/direct?from=82&to=35390'
{"from":82,"to":35390,"direct":false}
```

### Logging

The microservice has the ability to log messages to a logfile and to the Unix syslog facility. To enable debug logging, the `debug.enabled` setting in the microservice main config file `etc/settings.conf` should be set to `true` *before starting up the microservice*. When running under Ubuntu Server or Arch Linux (not in a Docker container), logs can be seen and analyzed in an ordinary fashion, by `tail`ing the `log/bus.log` logfile:

```
$ tail -f log/bus.log
[2026-09-26][20:20:00][INFO ]  Server started on port 8765
[2026-09-26][20:20:10][DEBUG]  from=4838 | to=524987
[2026-09-26][20:20:10][DEBUG]  1 =  1 2 3 4 5 6 7 8 9 987 11 12 13 4987 415 ...
...
[2026-09-26][20:20:30][DEBUG]  from=82 | to=35390
[2026-09-26][20:20:30][DEBUG]  1 =  1 2 3 4 5 6 7 8 9 987 11 12 13 4987 415 ...
...
[2026-09-26][20:20:50][INFO ]  Server stopped
```

Messages registered by the Unix system logger can be seen and analyzed using the `journalctl` utility:

```
$ journalctl -f
...
Sep 26 20:20:00 <hostname> busd[<pid>]: Server started on port 8765
Sep 26 20:20:10 <hostname> busd[<pid>]: from=4838 | to=524987
Sep 26 20:20:30 <hostname> busd[<pid>]: from=82 | to=35390
Sep 26 20:20:50 <hostname> busd[<pid>]: Server stopped
```

Inside the running container logs might be queried also by `tail`ing the `log/bus.log` logfile:

```
/var/tmp/bus $ tail -f log/bus.log
[2026-09-28][17:50:00][INFO ]  Server started on port 8765
[2026-09-28][17:50:10][DEBUG]  from=4838 | to=524987
[2026-09-28][17:50:10][DEBUG]  1 =  1 2 3 4 5 6 7 8 9 987 11 12 13 4987 415 ...
...
[2026-09-28][17:50:30][DEBUG]  from=82 | to=35390
[2026-09-28][17:50:30][DEBUG]  1 =  1 2 3 4 5 6 7 8 9 987 11 12 13 4987 415 ...
...
```

And of course, Docker itself gives the possibility to read log messages by using the corresponding command for that:

```
$ sudo docker logs -f busc99
[2026-09-28][17:50:00][INFO ]  Server started on port 8765
[2026-09-28][17:50:10][DEBUG]  from=4838 | to=524987
[2026-09-28][17:50:10][DEBUG]  1 =  1 2 3 4 5 6 7 8 9 987 11 12 13 4987 415 ...
...
[2026-09-28][17:50:30][DEBUG]  from=82 | to=35390
[2026-09-28][17:50:30][DEBUG]  1 =  1 2 3 4 5 6 7 8 9 987 11 12 13 4987 415 ...
...
[2026-09-28][17:50:50][INFO ]  Server stopped
```

### Error handling

When the query string passed in a request, contains inappropriate input, or the URI endpoint doesn't contain anything else at all after its path, the microservice will respond with the **HTTP 400 Bad Request** status code, including a specific response body in JSON representation, like the following:

```
$ curl 'http://localhost:8765/route/direct?from=qwerty4838&to=-i-.;--089asdf../nj524987'
{"error":"Request parameters must take positive integer values, in the range 1 .. 2,147,483,647. Please check your inputs."}
```

Or even simpler:

```
$ curl http://localhost:8765/route/direct
{"error":"Request parameters must take positive integer values, in the range 1 .. 2,147,483,647. Please check your inputs."}
```
