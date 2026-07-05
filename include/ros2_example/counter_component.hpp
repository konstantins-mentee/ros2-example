// Component node: publishes an incrementing counter on a timer.
//
// This header declares the class so it can be (a) registered as a loadable
// ROS 2 component and (b) linked directly into a standalone executable.
#ifndef ROS2_EXAMPLE__COUNTER_COMPONENT_HPP_
#define ROS2_EXAMPLE__COUNTER_COMPONENT_HPP_

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

namespace ros2_example
{

class CounterComponent : public rclcpp::Node
{
public:
  // Components MUST accept a NodeOptions and forward it to the base Node.
  // The component container passes options (remaps, params, use_intra_process)
  // in through this constructor.
  explicit CounterComponent(const rclcpp::NodeOptions & options);

private:
  void on_timer();

  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int32_t count_;
};

}  // namespace ros2_example

#endif  // ROS2_EXAMPLE__COUNTER_COMPONENT_HPP_
