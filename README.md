# Trans-RoutE-Townish (transroutownish) :small_orange_diamond: Urban bus routing microservice prototype (C port)

**A daemon written in C (GNOME/libsoup), designed and intended to be run as a microservice,
<br />implementing a simple urban bus routing prototype**

**Rationale:** This project is a *direct* **C99** port of the earlier developed **urban bus routing prototype**, written in Groovy using **[Ratpack](https://ratpack.io "A set of Java libraries for building scalable HTTP applications")** Java libraries, and tailored to be run as a microservice in a Docker container. The following description of the underlying architecture and logics has been taken **[from here](https://github.com/rgolubtsov/transroutownish-proto-bus-groovy)** as is, without any modifications or adjustment.

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
* **[Running](#running)**
* **[Consuming](#consuming)**
  * **[Logging](#logging)**
  * **[Error handling](#error-handling)**

## Building

The microservice might be built and run successfully under **Ubuntu Server (Ubuntu 22.04.4 LTS x86-64)** and **Arch Linux**. Install the necessary dependencies (`build-essential`, `tcc`, `libsoup-3.0-dev`, `docker.io`):

* In Ubuntu Server:

```
$ sudo apt-get update && \
  sudo apt-get install build-essential tcc libsoup-3.0-dev docker.io -y
...
```

* In Arch Linux:

```
$ sudo pacman -Syu base-devel tcc libsoup3 docker
...
```

**Build** the microservice using **GNU Make**:

```
$ make clean
rm -f -vR bin src/bus-core.o src/bus-controller.o src/bus-handler.o src/bus-helper.o
$
$ make all  # <== Building the daemon.
cc -Wall -std=c99 -march=x86-64 -O3 -pipe -c `pkg-config --cflags-only-I libsoup-3.0 json-glib-1.0` src/bus-core.c -o src/bus-core.o
cc -Wall -std=c99 -march=x86-64 -O3 -pipe -c `pkg-config --cflags-only-I libsoup-3.0 json-glib-1.0` src/bus-controller.c -o src/bus-controller.o
cc -Wall -std=c99 -march=x86-64 -O3 -pipe -c `pkg-config --cflags-only-I libsoup-3.0 json-glib-1.0` src/bus-handler.c -o src/bus-handler.o
cc -Wall -std=c99 -march=x86-64 -O3 -pipe -c `pkg-config --cflags-only-I libsoup-3.0 json-glib-1.0` src/bus-helper.c -o src/bus-helper.o
if [ ! -d bin ]; then \
    mkdir bin; \
fi
tcc `pkg-config   --libs-only-l libsoup-3.0 json-glib-1.0` -o bin/busd src/bus-core.o src/bus-controller.o src/bus-handler.o src/bus-helper.o
```

## Running

**Run** the microservice using its executable directly, built previously by the `all` target:

```
$ ./bin/busd; echo $?
...
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
...
```

The direct route is not found:

```
$ curl 'http://localhost:8765/route/direct?from=82&to=35390'
{"from":82,"to":35390,"direct":false}
...
```

### Logging

The microservice has the ability to log messages to a logfile and to the Unix syslog facility. When running under Ubuntu Server or Arch Linux (not in a Docker container), logs can be seen and analyzed in an ordinary fashion, by `tail`ing the `log/bus.log` logfile:

```
$ tail -f log/bus.log
...
[2024-09-03][22:40:10][INFO ]  Server started on port 8765
[2024-09-03][22:40:20][DEBUG]  from=4838 | to=524987
[2024-09-03][22:40:20][DEBUG]  1 =  1 2 3 4 5 6 7 8 9 987 11 12 13 4987 415 ...
...
[2024-09-03][22:40:41][DEBUG]  from=82 | to=35390
[2024-09-03][22:40:41][DEBUG]  1 =  1 2 3 4 5 6 7 8 9 987 11 12 13 4987 415 ...
...
[2024-09-03][22:40:51][INFO ]  Server stopped
```

Messages registered by the Unix system logger can be seen and analyzed using the `journalctl` utility:

```
$ journalctl -f
...
Sep 03 22:40:10 <hostname> busd[<pid>]: Server started on port 8765
Sep 03 22:40:20 <hostname> busd[<pid>]: from=4838 | to=524987
Sep 03 22:40:41 <hostname> busd[<pid>]: from=82 | to=35390
Sep 03 22:40:51 <hostname> busd[<pid>]: Server stopped
```

**TBD** :cd:

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
