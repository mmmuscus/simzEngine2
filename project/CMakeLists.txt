# Referencing: https://www.youtube.com/watch?v=ZuHK_5cJ6B8
# Referencing: https://www.youtube.com/watch?v=xYN3J13rjhM&list=PLalVdRk2RC6o5GHu618ARWh0VO0bFlif4&index=12

# TODO: Move the below to different files
# START OF WINDOWS ENVIRONMENT SPECIFIC VARIABLES
set(VULKAN_SDK_PATH C:/VulkanSDK/1.3.239.0)
set(GLFW_PATH C:/devTools/glfw-3.3.8.bin.WIN64)
set(GLM_PATH C:/devTools/glm)
# END OF WINDOWS SPECIFIC ENVIRONMENT VARIABLES

cmake_minimum_required (VERSION 3.15)

set (NAME SimzEngine2)
project(${NAME} VERSION 0.1.0)

message(STATUS "using ${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()
    set(USE_MINGW "True")
    set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER  ${MINGW_PATH}/bin/g++.exe)
endif()

# 1. Set VULKAN_SDK_PATH in .env.cmake to target specific vulkan version
# TODO: Fix
find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
#if (DEFINED VULKAN_SDK_PATH)
#  set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
#  set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
#  set(Vulkan_FOUND "True")
#else()
#  find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
#  message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
#endif()
#if (NOT Vulkan_FOUND)
#    message(FATAL_ERROR "Could not find Vulkan library!")
#else()
#    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
#endif()

#2. Set GLFW_PATH in .env.cmake to target specific glfw
if (DEFINED GLFW_PATH)
  message(STATUS "Using GLFW path specified in .env")
  set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
  if (MSVC)
    set(GLFW_LIB "${GLFW_PATH}/lib-vc2019") # 2.1 Update lib-vc2019 to use same version as your visual studio
  elseif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    message(STATUS "USING MINGW")
    set(GLFW_LIB "${GLFW_PATH}/lib-mingw-w64") # 2.1 make sure matches glfw mingw subdirectory
  endif()
else()
# Reference for below line: https://stackoverflow.com/questions/54834826/cannot-find-package-glfw-using-cmake
  list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
  find_package(glfw3 3.3 REQUIRED)
  set(GLFW_LIB glfw)
  message(STATUS "Found GLFW")
endif()
if (NOT GLFW_LIB)
    message(FATAL_ERROR "Could not find glfw library!")
else()
    message(STATUS "Using glfw lib at: ${GLFW_LIB}")
endif()

# 3. Adding external directories
include_directories(external)

set(IMGUI_PATH external/imGui)
set(STB_PATH external/stb)
set(TINYOBJLOADER_PATH external/tinyobjloader)

include_directories(engine)
# TODO: This is apparently not a very nice solution
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/engine/*.cpp)
file(GLOB_RECURSE IMGUI_SOURCES ${PROJECT_SOURCE_DIR}/external/imGui/*.cpp)
add_executable(${PROJECT_NAME} "main.cpp" ${SOURCES} ${IMGUI_SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${IMGUI_PATH}
    ${STB_PATH}
    ${TINYOBJLOADER_PATH}
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${Vulkan_LIBRARIES}
    ${GLFW_LIB}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw3 
    ${GLFW_LIB}
    ${Vulkan_LIBRARIES}
)