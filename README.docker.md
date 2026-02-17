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

### Permissions on Linux

If you are running on Linux, files created by the container will be owned by the user inside the container (default UID 1000). If your host user has a different UID, you might encounter permission issues.

To solve this, you can pass your host user's UID and GID to the container using the `HOST_UID` and `HOST_GID` environment variables. The container will automatically configure the internal `scrcpy` user to match these IDs.

```bash
docker run --rm -v $(pwd):/app -e HOST_UID=$(id -u) -e HOST_GID=$(id -g) scrcpy-env
```

If you have already created files with root ownership (or wrong ownership) from a previous run, you may need to fix the permissions on your host machine first:

```bash
sudo chown -R $(id -u):$(id -g) .
```

To build and extract the binary with correct permissions:

```bash
docker run --rm -v $(pwd):/app -e HOST_UID=$(id -u) -e HOST_GID=$(id -g) scrcpy-env /bin/bash -c "meson setup build --buildtype=release --strip -Db_lto=true && ninja -C build"
```

## Running Release Scripts

The container includes dependencies for running the release scripts (like cross-compilation for Windows).

To run a specific release script, for example to build the Windows release:

```bash
docker run --rm -v $(pwd):/app -e HOST_UID=$(id -u) -e HOST_GID=$(id -g) scrcpy-env ./release/build_windows.sh 64
```

To run the full release process (which builds for Linux and Windows, and packages everything):

```bash
docker run --rm -v $(pwd):/app -e HOST_UID=$(id -u) -e HOST_GID=$(id -g) scrcpy-env ./release/release.sh
```

The release artifacts will be generated in the `release/output` directory.

## Running Tests

To run the tests inside the container:

```bash
docker run --rm -v $(pwd):/app -e HOST_UID=$(id -u) -e HOST_GID=$(id -g) scrcpy-env ./gradlew test
```

## Interactive Shell

To start an interactive shell inside the container:

```bash
docker run --rm -it -v $(pwd):/app -e HOST_UID=$(id -u) -e HOST_GID=$(id -g) scrcpy-env /bin/bash
```
