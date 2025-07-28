cmake_minimum_required(VERSION 3.10)
project(stonefish_silver)

# Find necessary ROS2 and Stonefish packages
find_package(stonefish_ros2 REQUIRED)
find_package(Stonefish REQUIRED) # This is for the core Stonefish library

# Install the launch directory
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)

# Install the data directory to the package's share folder
install(DIRECTORY
  data
  DESTINATION share/${PROJECT_NAME}/
)

# Install the scenarios directory to the package's share folder
install(DIRECTORY
  scenarios
  DESTINATION share/${PROJECT_NAME}/
)

# This command is crucial for ROS2 packages
ament_package()