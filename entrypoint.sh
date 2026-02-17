#!/bin/bash
set -e

# Use provided HOST_UID and HOST_GID, or fallback to default scrcpy user's ID
# The scrcpy user is created with UID 1000 by default in the Dockerfile
PUID=${HOST_UID:-1000}
PGID=${HOST_GID:-1000}

echo "Starting with UID: $PUID, GID: $PGID"

# Modify the scrcpy user to match the host user's UID and GID
# We use -o to allow non-unique IDs in case the ID already exists
groupmod -o -g "$PGID" scrcpy
usermod -o -u "$PUID" scrcpy

# Ensure scrcpy owns its home directory
chown -R scrcpy:scrcpy /home/scrcpy

# Execute the command as the scrcpy user
exec gosu scrcpy "$@"
