cmake_minimum_required(VERSION 3.8)
project(ros2_example)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
# Provides rclcpp_components_register_node() and the component_container executables.
find_package(rclcpp_components REQUIRED)

# hello executable
add_executable(hello src/hello_node.cpp)
ament_target_dependencies(hello rclcpp)

# zero_copy_pubsub: producer/consumer demo using loaned messages
add_executable(zero_copy_pubsub src/zero_copy_pubsub.cpp)
ament_target_dependencies(zero_copy_pubsub rclcpp std_msgs)

install(TARGETS
    hello
    zero_copy_pubsub
  DESTINATION lib/${PROJECT_NAME})

# -----------------------------------------------------------
# Component example: shared library + registered component + standalone exe
# -----------------------------------------------------------
# 1. Core logic as a SHARED library so it can be loaded at runtime.
add_library(counter_component SHARED src/counter_component.cpp)
target_include_directories(counter_component PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(counter_component rclcpp rclcpp_components std_msgs)

# 2. Register as a ROS 2 component. This also generates a thin executable
#    named `counter_component_node` that spins the component standalone.
rclcpp_components_register_node(counter_component
  PLUGIN "ros2_example::CounterComponent"
  EXECUTABLE counter_component_node)

# 3. A hand-written standalone executable that links the same library.
add_executable(counter_main src/counter_main.cpp)
target_link_libraries(counter_main counter_component)
ament_target_dependencies(counter_main rclcpp)

# 4. Install the library and both executables.
#    - ARCHIVE (.a) / LIBRARY (.so) go to lib so the component loader finds them.
#    - RUNTIME (executables) go to lib/${PROJECT_NAME} so `ros2 run` finds them.
install(TARGETS
    counter_component
    counter_main
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME})

# Install the public headers.
install(DIRECTORY include/
  DESTINATION include)

# Install launch files and configs into the package share directory.
install(DIRECTORY
    launch
    config
  DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
