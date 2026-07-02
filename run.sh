#!/usr/bin/env bash
# Launch the ROS 2 dev container with ~/Projects mapped in, then build+run the
# hello example. With no args: builds and runs. Pass a command to run instead.
#
#   ./run.sh              # colcon build ros2_example, then `ros2 run` hello
#   ./run.sh bash         # just drop into an interactive shell
set -euo pipefail

IMAGE="${IMAGE:-ros2-jazzy-arm64:dev}"
PROJECTS_DIR="$(cd "$(dirname "$0")/.." && pwd)"   # = ~/Projects

if [ "$#" -gt 0 ]; then
  CMD=("$@")
else
  CMD=(bash -lc '
    set -e
    cd /home/ros/ws
    colcon build --packages-select ros2_example
    source install/setup.bash
    ros2 run ros2_example hello
  ')
fi

docker run --rm -it \
  --platform=linux/arm64 \
  --name ros2-example \
  --network host \
  --ipc host \
  -e ROS_DOMAIN_ID="${ROS_DOMAIN_ID:-0}" \
  -v "${PROJECTS_DIR}:/home/ros/ws/src" \
  -w /home/ros/ws \
  "${IMAGE}" \
  "${CMD[@]}"
