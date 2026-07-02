#!/usr/bin/env bash
# Launch the ROS 2 dev container with ~/Projects mapped in, then build+run the
# hello example. The image + platform are chosen automatically for the host
# architecture (override with IMAGE=... / PLATFORM=...).
#
#   ./run.sh              # colcon build ros2_example, then `ros2 run` hello
#   ./run.sh bash         # just drop into an interactive shell
set -euo pipefail

# Pick image + platform for the host arch (matches .devcontainer/select-image.sh).
case "$(uname -m)" in
  x86_64|amd64)
    IMAGE_DEFAULT="nvcr.io/nvidia/isaac/ros:x86_64-ros2_humble_f247dd1051869171c3fc53bb35f6b907"
    PLATFORM_DEFAULT="linux/amd64"
    ;;
  aarch64|arm64)
    IMAGE_DEFAULT="ros2-jazzy-arm64:dev"
    PLATFORM_DEFAULT="linux/arm64"
    ;;
  *)
    echo "run.sh: unsupported host arch '$(uname -m)'" >&2
    exit 1
    ;;
esac
IMAGE="${IMAGE:-$IMAGE_DEFAULT}"
PLATFORM="${PLATFORM:-$PLATFORM_DEFAULT}"

PROJECTS_DIR="$(cd "$(dirname "$0")/.." && pwd)"   # = ~/Projects

# Source whichever ROS underlay the image ships (Humble or Jazzy).
SOURCE_ROS='for f in /opt/ros/humble/setup.bash /opt/ros2_jazzy/install/setup.bash; do [ -f "$f" ] && . "$f" && break; done'

if [ "$#" -gt 0 ]; then
  CMD=("$@")
else
  CMD=(bash -lc "
    set -e
    $SOURCE_ROS
    cd /root/ws
    colcon build --packages-select ros2_example
    source install/setup.bash
    ros2 run ros2_example hello
  ")
fi

docker run --rm -it \
  --platform="${PLATFORM}" \
  --name ros2-example \
  --network host \
  --ipc host \
  -e ROS_DOMAIN_ID="${ROS_DOMAIN_ID:-0}" \
  -v "${PROJECTS_DIR}:/root/ws/src" \
  -w /root/ws \
  "${IMAGE}" \
  "${CMD[@]}"
