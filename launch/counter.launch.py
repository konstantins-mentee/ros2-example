"""Launch the CounterComponent inside a component container.

Demonstrates loading the registered component (ros2_example::CounterComponent)
into a generic container_mt process, with parameters from the installed
config/counter_params.yaml.
"""
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode


def generate_launch_description():
    params_file = os.path.join(
        get_package_share_directory("ros2_example"),
        "config",
        "counter_params.yaml",
    )

    container = ComposableNodeContainer(
        name="counter_container",
        namespace="",
        package="rclcpp_components",
        executable="component_container_mt",
        composable_node_descriptions=[
            ComposableNode(
                package="ros2_example",
                plugin="ros2_example::CounterComponent",
                name="counter_component",
                parameters=[params_file],
            ),
        ],
        output="screen",
    )

    return LaunchDescription([container])
