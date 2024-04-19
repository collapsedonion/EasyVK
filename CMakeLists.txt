cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

if(WIN32)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

find_package(Vulkan REQUIRED)

set(EASY_VULKAN_PATH ${CMAKE_SOURCE_DIR}/EasyVulkan)
set(EASY_VULKAN_INCLUDE ${EASY_VULKAN_PATH}/include/)

project(easy_vulkan)

FILE(GLOB EASY_VULKAN_SOURCE ${EASY_VULKAN_PATH}/source/*.cpp)
add_library(easy_vulkan ${EASY_VULKAN_SOURCE} ${EASY_VULKAN_INCLUDE_FILES})

if(NOT Vulkan_FOUND)
    message(ERROR "Failed to find Vulkan lib")
endif()

target_include_directories(easy_vulkan PRIVATE ${EASY_VULKAN_INCLUDE})
target_link_libraries(easy_vulkan PUBLIC Vulkan::Vulkan)

project(easy_vulkan_test)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/test)
find_package(glfw3 REQUIRED)

add_executable(easy_vulkan_test ${CMAKE_SOURCE_DIR}/test/source/main.cpp)
target_include_directories(easy_vulkan_test PRIVATE ${EASY_VULKAN_INCLUDE})
target_link_libraries(easy_vulkan_test PRIVATE easy_vulkan glfw Vulkan::Vulkan)
