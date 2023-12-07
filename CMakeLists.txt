cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(EASY_VULKAN_PATH ${CMAKE_SOURCE_DIR}/EasyVulkan)

project(easy_vulkan)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/EasyVulkan/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${EASY_VULKAN_PATH}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/EasyVulkan/lib)

FILE(GLOB EASY_VULKAN_SOURCE ${EASY_VULKAN_PATH}/source/*.cpp)

add_library(easy_vulkan ${EASY_VULKAN_SOURCE})

target_include_directories(easy_vulkan PRIVATE ${CMAKE_SOURCE_DIR}/EasyVulkan/include/)
target_include_directories(easy_vulkan PRIVATE ${CMAKE_SOURCE_DIR}/EasyVulkan/include/EasyVulkan)
target_include_directories(easy_vulkan PRIVATE /usr/local/include)
target_link_directories(easy_vulkan PRIVATE /usr/local/lib/)
target_link_libraries(easy_vulkan vulkan)

project(easy_vulkan_test)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/test)
add_executable(easy_vulkan_test ${CMAKE_SOURCE_DIR}/test/source/main.cpp)
target_include_directories(easy_vulkan_test PRIVATE ${CMAKE_SOURCE_DIR}/EasyVulkan/include /opt/homebrew/include)
target_link_directories(easy_vulkan_test PRIVATE /usr/local/lib/ /opt/homebrew/lib)
target_link_libraries(easy_vulkan_test easy_vulkan glfw)
target_include_directories(easy_vulkan_test PRIVATE /usr/local/include)
