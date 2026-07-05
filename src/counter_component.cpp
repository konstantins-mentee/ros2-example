// Implementation of CounterComponent, plus the macro that registers it as a
// dynamically-loadable ROS 2 component.
#include "ros2_example/counter_component.hpp"

#include <chrono>

using namespace std::chrono_literals;

namespace ros2_example
{

CounterComponent::CounterComponent(const rclcpp::NodeOptions & options)
: Node("counter_component", options), count_(0)
{
  // A parameter so the publish rate can be set from the command line, a launch
  // file, or a YAML config (see config/counter_params.yaml).
  const double rate_hz = this->declare_parameter<double>("rate_hz", 2.0);

  publisher_ = this->create_publisher<std_msgs::msg::Int32>("counter", 10);

  const auto period =
    std::chrono::duration<double>(1.0 / rate_hz);
  timer_ = this->create_wall_timer(
    std::chrono::duration_cast<std::chrono::nanoseconds>(period),
    std::bind(&CounterComponent::on_timer, this));

  RCLCPP_INFO(this->get_logger(), "CounterComponent up, publishing at %.1f Hz", rate_hz);
}

void CounterComponent::on_timer()
{
  std_msgs::msg::Int32 msg;
  msg.data = count_++;
  RCLCPP_INFO(this->get_logger(), "Publishing: %d", msg.data);
  publisher_->publish(msg);
}

}  // namespace ros2_example

// Registers the node with class_loader so it can be loaded into a running
// component container (e.g. `ros2 component load ...`). CMake's
// rclcpp_components_register_node() also generates a standalone executable
// from this same registration.
#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(ros2_example::CounterComponent)
