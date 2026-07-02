// Minimal rclcpp node: bring up ROS 2, log a hello message, shut down.
#include "rclcpp/rclcpp.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>("hello_node");
  RCLCPP_INFO(node->get_logger(), "Hello from ros2-example (rclcpp / Jazzy)!");

  rclcpp::shutdown();
  return 0;
}
