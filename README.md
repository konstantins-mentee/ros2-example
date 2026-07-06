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

## Executables

The package builds these targets:

- **`hello`** — minimal node that logs a hello message and exits.
- **`zero_copy_pubsub`** — a producer/consumer demo. The producer publishes a
  `std_msgs/Float64` at 10 Hz using `borrow_loaned_message()` (zero-copy loaned
  messages), and the consumer logs each value. Both nodes run in one
  `SingleThreadedExecutor`. Run it (Ctrl+C to stop):

  ```bash
  ros2 run ros2_example zero_copy_pubsub
  ```

- **`counter_component`** (library) + **`counter_component_node`** /
  **`counter_main`** (executables) — the ROS 2 *component* pattern. The counter
  logic lives in a `SHARED` library (`libcounter_component.so`), registered as a
  loadable component via `rclcpp_components_register_node()`. It publishes an
  incrementing `std_msgs/Int32` on `counter` at a configurable `rate_hz`.
  See [Running the component example](#running-the-component-example) below.

  Note on zero-copy: loaned messages only avoid a copy when the middleware
  supports shared memory (e.g. Fast DDS with SHM, or Iceoryx). Otherwise the
  call transparently falls back to a normal heap allocation — the code is
  identical either way. The container already runs with `--ipc=host` so the
  shared-memory transport can work.

## Running the component example

The counter is built three ways. **The first two are plain native executables**
— an ordinary ELF binary you can run directly, no component manager involved.
The third is the "true component" path: load the shared library into a running
container process at runtime.

All commands assume you have already built and sourced the overlay in the
container:

```bash
colcon build --packages-select ros2_example
source install/setup.bash
```

(For a copy-paste one-liner that does build + source + run in a single
throwaway container, see [One-shot native run](#one-shot-native-run) below —
needed because the `install/` tree lives at `/home/ros/ws`, which is **not**
mounted back to the host, so it must be built and used in the same
`docker run`.)

### 1. `counter_main` — your own native executable

The hand-written `main()` in `src/counter_main.cpp` that links the library
directly (`target_link_libraries`). It's a normal binary, so either of these
runs the **same** program — pick one:

```bash
ros2 run ros2_example counter_main                          # via the ros2 CLI
./install/ros2_example/lib/ros2_example/counter_main        # by path, identical
```

`ros2 run PKG EXE` just locates the executable inside the package's
`lib/ros2_example/` and execs it — there is nothing extra happening versus
running the file directly. Use `ros2 run`; the raw path is only shown to make
the point that it's an ordinary native binary.

### 2. `counter_component_node` — native executable, auto-generated

`rclcpp_components_register_node(... EXECUTABLE counter_component_node)`
generates a thin binary that loads the component into its own process for you.
Also fully native, and it takes parameters:

```bash
ros2 run ros2_example counter_component_node --ros-args -p rate_hz:=10.0
```

`counter_main` (which you wrote) and `counter_component_node` (generated from
the `RCLCPP_COMPONENTS_REGISTER_NODE` macro) run the exact same library logic —
the only difference is who wrote the `main()`.

### 3. Load into a component container (the "real" component use)

Only needed when you actually want runtime composition — e.g. several
components sharing one process for zero-copy intra-process comms. Not native
standalone; the library is loaded at runtime.

```bash
# terminal A: start an empty container process
ros2 run rclcpp_components component_container_mt

# terminal B: hot-load the component into it, then inspect
ros2 component load /ComponentManager ros2_example ros2_example::CounterComponent
ros2 component list
```

Or declaratively — the launch file starts a container **and** loads the
component with parameters from `config/counter_params.yaml` (`rate_hz: 5.0`):

```bash
ros2 launch ros2_example counter.launch.py
```

### One-shot native run

From the host, build + source + run a native executable in one throwaway
container:

```bash
cd ~/Projects/ros2-example
PROJECTS_DIR="$(cd .. && pwd)"
docker run --rm -it --platform=linux/arm64 --network host --ipc host \
  -v "${PROJECTS_DIR}:/home/ros/ws/src" -w /home/ros/ws \
  ros2-jazzy-arm64:dev bash -lc '
    colcon build --packages-select ros2_example
    source install/setup.bash
    ros2 run ros2_example counter_main       # swap for any command above
  '
```

### Observing the output

While any of the above is running, from another sourced shell:

```bash
ros2 topic echo /counter          # see the published std_msgs/Int32
ros2 topic hz /counter            # confirm the rate matches rate_hz
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

## Develop inside the container with VS Code

This project ships a Dev Container so VS Code runs *inside* `ros2-jazzy-arm64:dev`
with the full ROS 2 toolchain, IntelliSense, build tasks and a debugger.

Prerequisites: [VS Code](https://code.visualstudio.com/) + the **Dev Containers**
extension (`ms-vscode-remote.remote-containers`), Docker running, and the
`ros2-jazzy-arm64:dev` image built.

Steps:

1. `code ~/Projects/ros2-example`
2. Command Palette → **Dev Containers: Reopen in Container** (VS Code reads
   `.devcontainer/devcontainer.json`, starts the image, and installs the C++,
   CMake, Python and ROS extensions in the container).
3. VS Code opens at `/home/ros/ws/src/ros2-example`, with all of `~/Projects`
   mounted at `/home/ros/ws/src`.

Inside the container:

- **Build:** `Ctrl/Cmd+Shift+B` (runs the `colcon build` task in `/home/ros/ws`).
- **Run:** Command Palette → *Tasks: Run Task* → **run hello** → logs the message.
- **Debug:** open `src/hello_node.cpp`, press `F5` (*Debug hello (gdb)*). It
  builds first, then launches the executable under gdb. The **ROS** extension
  auto-sources the workspace so the node's shared libraries resolve; if you
  debug without it, launch from the integrated terminal after
  `source install/setup.bash` instead.

What the config does:

- `.devcontainer/devcontainer.json` — uses the `ros2-jazzy-arm64:dev` image,
  `--platform=linux/arm64`, host networking + IPC, runs as user `ros`, and
  mounts `${localWorkspaceFolder}/..` (i.e. `~/Projects`) at `/home/ros/ws/src`.
- `.vscode/tasks.json` — `colcon build`, `run hello`, and `clean` tasks.
- `.vscode/launch.json` — gdb debug config for the `hello` executable.
- `.vscode/c_cpp_properties.json` + `settings.json` — IntelliSense include paths
  pointing at the underlay (`/opt/ros2_jazzy/install`) and overlay
  (`/home/ros/ws/install`); C++17; `cmake.configureOnOpen` disabled so VS Code
  doesn't try to configure against the host toolchain.
