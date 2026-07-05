// Standalone entry point: constructs the CounterComponent directly and spins
// it, without any component container. This demonstrates the "link the library
// into your own executable" path (target_link_libraries in CMakeLists.txt).
#include "ros2_example/counter_component.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ros2_example::CounterComponent>(rclcpp::NodeOptions()));
  rclcpp::shutdown();
  return 0;
}
