cmake_minimum_required(VERSION 3.24)
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
set(LIBRARY_OUTPUT_PATH ${EASY_VULKAN_PATH}/lib)


if(NOT Vulkan_FOUND)
    message(ERROR "Failed to find Vulkan lib")
endif()

target_include_directories(easy_vulkan PUBLIC ${EASY_VULKAN_INCLUDE})
target_link_libraries(easy_vulkan PUBLIC Vulkan::Vulkan)
