#!/usr/bin/env bash
# Pick the ROS 2 dev image + Docker platform for the *host* architecture and
# write them to ../.env, which docker compose (and thus the dev container and
# run.sh) reads for ${ROS2_IMAGE} / ${ROS2_PLATFORM}.
#
#   x86_64  -> NVIDIA Isaac ROS Humble (ready-to-use)   linux/amd64
#   aarch64 -> ros2-jazzy-arm64:dev (locally built)     linux/arm64
#
# Runs on the host as the dev container `initializeCommand`, before the
# container is created.
set -euo pipefail

env_file="$(cd "$(dirname "$0")/.." && pwd)/.env"

case "$(uname -m)" in
  x86_64 | amd64)
    image="nvcr.io/nvidia/isaac/ros:x86_64-ros2_humble_f247dd1051869171c3fc53bb35f6b907"
    platform="linux/amd64"
    ;;
  aarch64 | arm64)
    image="ros2-jazzy-arm64:dev"
    platform="linux/arm64"
    ;;
  *)
    echo "select-image: unsupported host arch '$(uname -m)'" >&2
    exit 1
    ;;
esac

printf 'ROS2_IMAGE=%s\nROS2_PLATFORM=%s\n' "$image" "$platform" > "$env_file"
echo "select-image: $(uname -m) -> $image ($platform)"
