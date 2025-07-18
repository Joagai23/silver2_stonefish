# CMakeLists.txt for My Silver Stonefish Simulation

# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(SilverStonefish LANGUAGES CXX)

# Specify the C++ standard to use
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # For strict standard compliance

# Add the current directory to the include path so SilverManager.h can be found
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Find the Stonefish package
find_package(Stonefish REQUIRED)

# Add the executable target
# The source files for your application
add_executable(${PROJECT_NAME}
    main.cpp
    SilverManager.cpp
)

# Link the executable with the Stonefish library
# Stonefish::Stonefish is the imported target name provided by find_package(Stonefish)
target_link_libraries(${PROJECT_NAME}
    Stonefish::Stonefish
)

