cmake_minimum_required(VERSION 3.10)
project(stonefish_silver)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find necessary ROS2 and Stonefish packages
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(stonefish_ros2 REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(Stonefish REQUIRED) # This is for the core Stonefish library

# Add the executable with source files from the src/ directory
add_executable(silver_simulation_node
  src/main.cpp
  src/SilverManager.cpp
)

# Add dependencies from other ROS packages (includes and libraries)
ament_target_dependencies(silver_simulation_node
  rclcpp
  stonefish_ros2
  ament_index_cpp
)

# Add the include directories for this package and its dependencies.
# This explicitly tells the compiler where to find the header files.
target_include_directories(silver_simulation_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${stonefish_ros2_INCLUDE_DIRS}
)

# Link the core Stonefish library
target_link_libraries(silver_simulation_node
  Stonefish::Stonefish
)

# Install the headers from the include/ directory
install(
  DIRECTORY include/
  DESTINATION include
)

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

# Install the executable
install(TARGETS
  silver_simulation_node
  DESTINATION lib/${PROJECT_NAME}
)

# This command is crucial for ROS2 packages
ament_package()