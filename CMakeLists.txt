# @file
# @author Erwin Meza Vega <emezav@unicauca.edu.co>
# @copyright 2020 Onwards MIT License
# Configuration file for CMake - C++ 17 base project

# Minimum required version: 3.24 
cmake_minimum_required(VERSION 3.24)

# Project name - Change executable name filename to current project dirname
# Use current directory as project name
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME ProjectName)
string(REPLACE " " "_" ProjectId ${ProjectName})

project(${ProjectName} VERSION 0.9 LANGUAGES CXX)

# To use a custom project name:
# comment the project(...) line before this comment,
# replace MYPROJECT with the desired project name and uncomment the following line:
#project("MYPROJECT" LANGUAGES <INSERT LANGUAGES HERE> )

# Set position independent code to ON, required for some libraries
# Please check carefully for each library required on the project
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Use folders to group CMake targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# Configure external package dependencies
set(EXTERNAL_PACKAGES
  "geo|https://github.com/emezav/geo.git"
)
set(EXTERNAL_PACKAGES_DIR "${CMAKE_SOURCE_DIR}/third_party")

# Download external packages if external packages dir does not exist
if(NOT EXISTS "${EXTERNAL_PACKAGES_DIR}")
  message(STATUS "Checking out third party packages...")
  file(MAKE_DIRECTORY ${EXTERNAL_PACKAGES_DIR})

  foreach(EXT_P IN LISTS EXTERNAL_PACKAGES)
    string(REPLACE "|" ";" EXT_REPO ${EXT_P})

    list(GET EXT_REPO 0 REPO_NAME)
    list(GET EXT_REPO 1 REPO_URL)

    if(NOT EXISTS "${EXTERNAL_PACKAGES_DIR}/${REPO_NAME}")
      message(STATUS "Cloning ${REPO_NAME} from ${REPO_URL}")
      # Fetch repo
      execute_process(
        COMMAND git clone ${REPO_URL} "${EXTERNAL_PACKAGES_DIR}/${REPO_NAME}"
        RESULT_VARIABLE CLONE_RESULT)
      if(CLONE_RESULT EQUAL 0)
        message(STATUS "${EXT_P} downloaded successfully.")
      else()
        message(FATAL_ERROR "Command failed with exit code: ${CMD_RESULT}")
      endif()
    else()
      message(STATUS "${REPO_NAME} already cloned.")
    endif()
  endforeach()

  # Copy files from third party packages

  # geo header
  file(COPY "${CMAKE_SOURCE_DIR}/third_party/geo/include/geo.h" DESTINATION "${CMAKE_SOURCE_DIR}/include")

endif()

# Add c/c++ headers and sources in src/ and include/folders
file(GLOB cpu_source_files "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/include/*.h")

# Add include directory "./include"
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add project default executable
add_executable(${PROJECT_NAME} ${cpu_source_files})

# Create Debug and Release dirs inside build directory
set_target_properties(${PROJECT_NAME}
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set C++ to std 17
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Set C++ standard required
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

# Disable compiler extensions beyond the C++17 standard
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

# Set default start project on Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Set link flags to mirror project link flags
set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_FLAGS ${PROJ_LINK_FLAGS})
