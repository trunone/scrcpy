# Docker Build Environment for Scrcpy

This directory contains a `Dockerfile` to create a Docker environment for building scrcpy.

## Prerequisites

- Docker installed and running.

## Building the Docker Image

To build the Docker image, run the following command from the root of the repository:

```bash
docker build -t scrcpy-env .
```

## Building Scrcpy

To build scrcpy inside the Docker container, run:

```bash
docker run --rm scrcpy-env
```

This will run the default command:
```bash
meson setup build --buildtype=release --strip -Db_lto=true && ninja -C build
```

The build artifacts will be inside the container. To persist them or access them from the host, you should mount the current directory:

```bash
docker run --rm -v $(pwd):/app scrcpy-env
```

However, note that the build directory created inside the container might have different permissions than your host user if you are on Linux. The Dockerfile creates a user `scrcpy` inside the container.

To build and extract the binary:

```bash
docker run --rm -v $(pwd):/app scrcpy-env /bin/bash -c "meson setup build --buildtype=release --strip -Db_lto=true && ninja -C build"
```

## Running Tests

To run the tests inside the container:

```bash
docker run --rm -v $(pwd):/app scrcpy-env ./gradlew test
```

## Interactive Shell

To start an interactive shell inside the container:

```bash
docker run --rm -it -v $(pwd):/app scrcpy-env /bin/bash
```
