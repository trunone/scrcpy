# Use Ubuntu 22.04 LTS as the base image
FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt-get update && apt-get install -y \
    curl \
    wget \
    unzip \
    git \
    python3 \
    python3-pip \
    gcc \
    pkg-config \
    meson \
    ninja-build \
    libsdl2-dev \
    libavcodec-dev \
    libavdevice-dev \
    libavformat-dev \
    libavutil-dev \
    libswresample-dev \
    libusb-1.0-0-dev \
    openjdk-17-jdk \
    mingw-w64 \
    nasm \
    zip \
    gosu \
    && rm -rf /var/lib/apt/lists/*

# Set up Android SDK environment variables
ENV ANDROID_HOME=/opt/android-sdk
ENV PATH=$PATH:$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools

# Download and install Android SDK Command Line Tools
# Use a specific version for reproducibility (build 11076708)
RUN mkdir -p $ANDROID_HOME/cmdline-tools \
    && curl -o cmdline-tools.zip https://dl.google.com/android/repository/commandlinetools-linux-11076708_latest.zip \
    && unzip cmdline-tools.zip -d $ANDROID_HOME/cmdline-tools \
    && mv $ANDROID_HOME/cmdline-tools/cmdline-tools $ANDROID_HOME/cmdline-tools/latest \
    && rm cmdline-tools.zip

# Install Android SDK components
# We need platform-tools, platforms;android-36, and build-tools;36.0.0
RUN yes | sdkmanager --licenses \
    && sdkmanager "platform-tools" "platforms;android-36" "build-tools;36.0.0"

# Create a non-root user 'scrcpy'
# This is necessary because Gradle often has issues running as root,
# and the build scripts explicitly check for EUID 0 to skip Gradle builds.
RUN useradd -m scrcpy

# Grant permissions to the Android SDK directory
# We use a group 'android_sdk' to allow the scrcpy user to access the SDK
# even if its UID/GID changes at runtime.
RUN groupadd -r android_sdk \
    && usermod -aG android_sdk scrcpy \
    && chown -R :android_sdk $ANDROID_HOME \
    && chmod -R g+rwX $ANDROID_HOME

# Set the working directory
WORKDIR /app

# Copy the entrypoint script
COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# Copy the project files into the container
# Use --chown to ensure the files are owned by the scrcpy user
COPY --chown=scrcpy:scrcpy . .

# Set the entrypoint
ENTRYPOINT ["/entrypoint.sh"]

# Define the default command to build the project
# We use a shell to execute multiple commands: setup and build
CMD ["/bin/bash", "-c", "meson setup build --buildtype=release --strip -Db_lto=true && ninja -C build"]
