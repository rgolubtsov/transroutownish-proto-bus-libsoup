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
  * **[Creating a Docker image](#creating-a-docker-image)**
* **[Running](#running)**
  * **[Running a Docker image](#running-a-docker-image)**
  * **[Exploring a Docker image payload](#exploring-a-docker-image-payload)**
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

### Creating a Docker image

**Build** a Docker image for the microservice:

```
$ # Pull the Alpine Linux image first, if not already there:
$ sudo docker pull alpine:edge
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

### Running a Docker image

**Run** a Docker image of the microservice, deleting all stopped containers prior to that:

```
$ sudo docker rm `sudo docker ps -aq`; \
  export PORT=8765 && sudo docker run -dp${PORT}:${PORT} --name busc99 transroutownish/busc99; echo $?
...
```

### Exploring a Docker image payload

The following is not necessary but might be considered interesting &mdash; to look up into the running container, and check out that the microservice's executable, config, log, and routes data store are at their expected places and in effect:

```
$ sudo docker ps -a
CONTAINER ID   IMAGE                    COMMAND      CREATED             STATUS             PORTS                                       NAMES
<container_id> transroutownish/busc99   "bin/busd"   About an hour ago   Up About an hour   0.0.0.0:8765->8765/tcp, :::8765->8765/tcp   busc99
$
$ sudo docker exec -it busc99 sh; echo $?
/var/tmp/bus $
/var/tmp/bus $ uname -a
Linux <container_id> 5.15.0-119-generic #129-Ubuntu SMP Fri Aug 2 19:25:20 UTC 2024 x86_64 Linux
/var/tmp/bus $
/var/tmp/bus $ ls -al
total 36
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:30 .
drwxrwxrwt    1 root     root          4096 Sep  5 19:21 ..
-rw-rw-r--    1 root     root          1443 Sep  5 18:40 Makefile
drwxr-xr-x    2 daemon   daemon        4096 Sep  5 19:22 bin
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:21 data
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:21 etc
drwxr-xr-x    2 daemon   daemon        4096 Sep  5 19:30 log
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:22 src
/var/tmp/bus $
/var/tmp/bus $ ls -al bin/ data/ etc/ log/ src/
bin/:
total 28
drwxr-xr-x    2 daemon   daemon        4096 Sep  5 19:22 .
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:30 ..
-rwxr-xr-x    1 daemon   daemon       20328 Sep  5 19:22 busd

data/:
total 56
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:21 .
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:30 ..
-rw-rw-r--    1 root     root         46218 Nov 13  2023 routes.txt

etc/:
total 12
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:21 .
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:30 ..
-rw-rw-r--    1 root     root           808 Sep  5 18:50 settings.conf

log/:
total 12
drwxr-xr-x    2 daemon   daemon        4096 Sep  5 19:30 .
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:30 ..
-rw-r--r--    1 daemon   daemon          59 Sep  5 19:30 bus.log

src/:
total 84
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:22 .
drwxr-xr-x    1 daemon   daemon        4096 Sep  5 19:30 ..
-rw-rw-r--    1 root     root          3311 Sep  5 18:40 bus-controller.c
-rw-r--r--    1 daemon   daemon        4064 Sep  5 19:22 bus-controller.o
-rw-rw-r--    1 root     root          4836 Sep  5 18:40 bus-core.c
-rw-r--r--    1 daemon   daemon        5968 Sep  5 19:22 bus-core.o
-rw-rw-r--    1 root     root          8452 Sep  5 18:40 bus-handler.c
-rw-r--r--    1 daemon   daemon        8728 Sep  5 19:22 bus-handler.o
-rw-rw-r--    1 root     root          7593 Sep  5 18:40 bus-helper.c
-rw-r--r--    1 daemon   daemon       10096 Sep  5 19:22 bus-helper.o
-rw-rw-r--    1 root     root          6075 Sep  5 18:40 busd.h
/var/tmp/bus $
/var/tmp/bus $ netstat -plunt
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name
tcp        0      0 0.0.0.0:8765            0.0.0.0:*               LISTEN      1/busd
tcp        0      0 :::8765                 :::*                    LISTEN      1/busd
/var/tmp/bus $
/var/tmp/bus $ ps ax
PID   USER     TIME  COMMAND
    1 daemon    0:00 bin/busd
    9 daemon    0:00 sh
   19 daemon    0:00 ps ax
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
