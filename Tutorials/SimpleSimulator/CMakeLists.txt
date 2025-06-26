# CMakeLists.txt for My First Stonefish Simulation

# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(SimpleSimulator LANGUAGES CXX)

# Specify the C++ standard to use
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # For strict standard compliance

# Add the current directory to the include path so MySimulationManager.h can be found
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Find the Stonefish package
# Stonefish should have installed a CMake package configuration file
# (e.g., StonefishConfig.cmake) when you ran `sudo make install`.
# This command tells CMake to find that configuration and use its variables
# for include directories and libraries.
find_package(Stonefish REQUIRED)

# Add the executable target
# The source files for your application
add_executable(${PROJECT_NAME}
    main.cpp
    MySimulationManager.cpp
)

# Link the executable with the Stonefish library
# Stonefish::Stonefish is the imported target name provided by find_package(Stonefish)
target_link_libraries(${PROJECT_NAME}
    Stonefish::Stonefish
)

# If Stonefish requires special compiler flags (e.g., -fopenmp),
# these are typically handled by the StonefishConfig.cmake.
# If you encounter issues, you might need to add:
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
# target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra) # Good practice for warnings