cmake_minimum_required(VERSION 3.8)
project(ros2_example)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

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

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
