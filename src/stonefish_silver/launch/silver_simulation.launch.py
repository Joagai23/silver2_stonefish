from launch_ros.substitutions import FindPackageShare
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node

def generate_launch_description():

    # Main Stonefish_ROS2 Simulator Launch Description
    stonefish_sim_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('stonefish_ros2'),
                'launch',
                'stonefish_simulator.launch.py'
            ])
        ]),
        launch_arguments = {
            'simulation_data' : PathJoinSubstitution([FindPackageShare('stonefish_silver'), 'data']),
            'scenario_desc' : PathJoinSubstitution([FindPackageShare('stonefish_silver'), 'scenarios', 'simulation.scn']),
            'simulation_rate' : '100.0',
            'window_res_x' : '1200',
            'window_res_y' : '900',
            'rendering_quality' : 'high'
        }.items()
    )

    # Locomotion Controller Node
    locomotion_node = Node(
        package="stonefish_silver",
        executable="cmd_vel_control_rotation.py",
        name="locomotion_controller",
        output="screen"
    )

    return LaunchDescription([
        stonefish_sim_launch,
        locomotion_node
    ])