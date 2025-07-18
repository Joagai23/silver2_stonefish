import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node

def generate_launch_description():
    # Get the path to the stonefish_ros2 package
    stonefish_pkg_share = get_package_share_directory('stonefish_ros2')

    # Define the path to the default Stonefish simulator launch file
    stonefish_launch_file = os.path.join(stonefish_pkg_share, 'launch', 'simulator.launch.py')

    # Include the Stonefish simulator launch description
    # This will start the simulator using its default settings.
    # You can pass arguments here to specify a scenario file, e.g.,
    # launch_arguments={'scenario': 'my_scenario.xml'}.items()
    stonefish_simulator_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(stonefish_launch_file)
    )

    # Define the publisher node from your package
    publisher_node = Node(
        package='my_basic_package',
        executable='publisher',
        name='basic_publisher'
    )

    # Define the subscriber node from your package
    subscriber_node = Node(
        package='my_basic_package',
        executable='subscriber',
        name='basic_subscriber'
    )

    # Return the complete launch description
    return LaunchDescription([
        stonefish_simulator_launch,
        publisher_node,
        subscriber_node
    ])