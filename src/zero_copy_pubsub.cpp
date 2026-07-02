#include <chrono>
#include <memory>
#include <utility>
#include <iostream>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
using namespace std::chrono_literals;
// ---------------------------------------------------------
// PRODUCER NODE
// ---------------------------------------------------------
class ZeroCopyProducer : public rclcpp::Node
{
public:
  ZeroCopyProducer() : Node("zero_copy_producer"), count_(0.0)
  {
    // Create the publisher
    publisher_ = this->create_publisher<std_msgs::msg::Float64>("zero_copy_topic", 10);
    // Timer to trigger the publish loop at 10Hz
    timer_ = this->create_wall_timer(
      100ms, std::bind(&ZeroCopyProducer::timer_callback, this));
  }
private:
  void timer_callback()
  {
    // 1. Borrow a chunk of memory directly from the middleware's shared pool
    // If the middleware does not support zero-copy, this transparently falls back
    // to standard heap allocation.
    auto loaned_msg = publisher_->borrow_loaned_message();
    // 2. Write data directly into the shared memory segment
    loaned_msg.get().data = count_;
    RCLCPP_INFO(this->get_logger(), "Publishing Loaned Data: '%f'", loaned_msg.get().data);
    count_ += 1.5;
    // 3. Publish the message by moving ownership back to the middleware.
    // The loaned_msg object is no longer valid after this call.
    publisher_->publish(std::move(loaned_msg));
  }
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  double count_;
};
// ---------------------------------------------------------
// CONSUMER NODE
// ---------------------------------------------------------
class ZeroCopyConsumer : public rclcpp::Node
{
public:
  ZeroCopyConsumer() : Node("zero_copy_consumer")
  {
    // The subscriber requires no special API changes.
    // By accepting a SharedPtr (or UniquePtr), the middleware passes the
    // shared memory reference directly to the callback without copying the payload.
    subscriber_ = this->create_subscription<std_msgs::msg::Float64>(
      "zero_copy_topic", 10,
      [this](const std_msgs::msg::Float64::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(), "Consumed Data via Zero-Copy: '%f'", msg->data);
      });
  }
private:
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr subscriber_;
};
// ---------------------------------------------------------
// MAIN
// ---------------------------------------------------------
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  // Run both nodes in a single executor for demonstration.
  // In a real-world scenario, these could be running in entirely separate
  // processes/terminals on the same computer.
  rclcpp::executors::SingleThreadedExecutor executor;

  auto producer = std::make_shared<ZeroCopyProducer>();
  auto consumer = std::make_shared<ZeroCopyConsumer>();
  executor.add_node(producer);
  executor.add_node(consumer);
  executor.spin();
  rclcpp::shutdown();
  return 0;
}
