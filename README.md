# ros2-example

A minimal ROS 2 **Jazzy** example package. It brings in the main client-library
dependency (`rclcpp`) and prints a hello message. It runs inside the prebuilt
ARM64 dev image `ros2-jazzy-arm64:dev`, with `~/Projects` mapped into the
container as the colcon workspace source.

> ROS 2 package names can't contain hyphens, so the folder is `ros2-example`
> but the package name is **`ros2_example`**.

## Layout

```
ros2-example/
├── package.xml           # ament_cmake package, depends on rclcpp
├── CMakeLists.txt        # builds the `hello` executable
├── src/hello_node.cpp    # rclcpp node that logs "Hello ..."
├── docker-compose.yml    # runs ros2-jazzy-arm64:dev, maps ~/Projects -> ws/src
├── run.sh                # one-shot: build + run hello in the container
└── .gitignore
```

## Prerequisites

The dev image must exist locally:

```bash
docker image inspect ros2-jazzy-arm64:dev >/dev/null && echo "image present"
# if not: build it from ~/ROS2_build (./build.sh)
```

## Quick start (one command)

```bash
cd ~/Projects/ros2-example
./run.sh
```

This mounts `~/Projects` at `/home/ros/ws/src`, runs `colcon build
--packages-select ros2_example`, sources the overlay, and executes the node.
Expected output:

```
[INFO] [hello_node]: Hello from ros2-example (rclcpp / Jazzy)!
```

## Interactive workflow

```bash
cd ~/Projects/ros2-example
docker compose run --rm ros2-example        # shell in /home/ros/ws
#   (or: ./run.sh bash)

# inside the container:
colcon build --packages-select ros2_example
source install/setup.bash
ros2 run ros2_example hello
```

Because the whole `~/Projects` tree is mounted as `src/`, any other ROS 2
package you drop under `~/Projects` is also picked up by `colcon build`.

## How the container mapping works

- Image: `ros2-jazzy-arm64:dev` (the source-built Jazzy core underlay).
- `~/Projects` → `/home/ros/ws/src` (bind mount; edits on the host are live).
- The image entrypoint auto-sources the underlay (`/opt/ros2_jazzy/install`)
  and, once built, your overlay (`/home/ros/ws/install`).
- Build output (`build/`, `install/`, `log/`) lands under `~/Projects` on the
  host and is git-ignored.
