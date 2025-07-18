from launch_ros.substitutions import FindPackageShare
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution

def generate_launch_description():
    return LaunchDescription([
        IncludeLaunchDescription(
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
                'simulation_rate' : '500.0',
                'window_res_x' : '1200',
                'window_res_y' : '900',
                'rendering_quality' : 'high'
            }.items()
        )
    ])