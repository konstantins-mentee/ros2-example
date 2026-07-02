#!/usr/bin/env bash
# Wire ROS into every interactive shell inside the container. Works on both
# images: the Isaac Humble image (/opt/ros/humble) does not auto-source ROS,
# and the arm64 Jazzy image (/opt/ros2_jazzy/install) may or may not. Whichever
# underlay exists is sourced; the overlay is sourced once it has been built.
set -e

underlay='for f in /opt/ros/humble/setup.bash /opt/ros2_jazzy/install/setup.bash; do [ -f "$f" ] && . "$f" && break; done'
overlay='[ -f /root/ws/install/setup.bash ] && . /root/ws/install/setup.bash'

for line in "$underlay" "$overlay"; do
  grep -qF "$line" ~/.bashrc 2>/dev/null || echo "$line" >> ~/.bashrc
done
