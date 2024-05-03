import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory


package_name = "ros2hwif"

def generate_launch_description():

    # equivalent to
    # $ xacro ~/agv_ws/install/ros2hwif/share/ros2hwif/description/robot.urdf.xacro sim_mode:=false
    robot_description_content = launch.substitutions.Command(
        [
            "xacro",
            " ",
            launch.substitutions.PathJoinSubstitution(
                [get_package_share_directory(package_name), 'description', 'robot.urdf.xacro']
            ),
            " ",
            "sim_mode:=false"
        ]
    )

    # equivalent to
    # $ ros2 run robot_state_publisher robot_state_publisher --ros-args -p robot_description:="$( xacro ~/agv_ws/install/ros2hwif/share/ros2hwif/description/robot.urdf.xacro )"
    robot_state_publisher = launch_ros.actions.Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': robot_description_content}]
    )

    # equivalent to
    # $ ros2 run controller_manager ros2_control_node --ros-args -p robot_description:="$( xacro ~/agv_ws/install/ros2hwif/share/ros2hwif/description/robot.urdf.xacro )" --params-file ~/agv_ws/install/ros2hwif/share/ros2hwif/config/diffbot_controllers.yaml
    controller_manager = launch_ros.actions.Node(
        package='controller_manager',
        executable='ros2_control_node',
        parameters=[
            {'robot_description': robot_description_content},
            launch.substitutions.PathJoinSubstitution(
                [get_package_share_directory(package_name), 'config', 'diffbot_controllers.yaml']
            )
        ]
    )
    delayed_controller_manager = launch.actions.TimerAction(period=1.0, actions=[controller_manager])


    # equivalent to
    # $ ros2 run controller_manager spawner.py diffbot_base_controller --controller-manager /controller_manager
    diffbot_base_controller_spawner = launch_ros.actions.Node(
        package='controller_manager',
        executable='spawner.py',
        arguments=['diffbot_base_controller']
    )

    # equivalent to
    # $ ros2 run controller_manager spawner.py joint_state_broadcaster --controller-manager /controller_manager
    joint_state_broadcaster_spawner = launch_ros.actions.Node(
        package='controller_manager',
        executable='spawner.py',
        arguments=['joint_state_broadcaster']
    )

    delayed_controller_spawners = launch.actions.RegisterEventHandler(
        event_handler=launch.event_handlers.OnProcessStart(
            target_action=controller_manager,
            on_start=[diffbot_base_controller_spawner, joint_state_broadcaster_spawner]
        )
    )


    return launch.LaunchDescription([
        robot_state_publisher,
        delayed_controller_manager,
        delayed_controller_spawners,
    ])