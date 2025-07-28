from launch_ros.substitutions import FindPackageShare
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, RegisterEventHandler
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from launch.event_handlers import OnProcessExit

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
            'simulation_rate' : '500.0',
            'window_res_x' : '1200',
            'window_res_y' : '900',
            'rendering_quality' : 'high'
        }.items()
    )

    # Joint State Broadcaster Spawner (Might not be needed)
    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster", "--controller-manager", "/controller_manager"],
    )

    # PID Position Controller Spawner
    pid_position_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["pid_position_controller", "--controller-manager", "/controller_manager"],
    )

    # Delay the PID controller until the joint_state_broadcaster is running
    delayed_pid_controller = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[pid_position_controller_spawner],
        )
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
        joint_state_broadcaster_spawner,
        delayed_pid_controller,
        locomotion_node
    ])