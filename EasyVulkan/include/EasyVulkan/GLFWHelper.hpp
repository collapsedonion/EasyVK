//
// Created by Роман  Тимофеев on 25.04.2024.
//

#ifndef EASY_VULKAN_TEST_GLFWHELPER_HPP
#define EASY_VULKAN_TEST_GLFWHELPER_HPP

#ifdef EASYVK_GLFW_HELPER

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace EasyVK{

    class GLFWHelper{
    public:
        inline static vk::SurfaceKHR getGLFWSurface(GLFWwindow* window, EasyVK::Instance instance){
            VkSurfaceKHR surface;
            glfwCreateWindowSurface(instance.instance, window, nullptr, &surface);
            return surface;
        }
    };
};

#endif

#endif //EASY_VULKAN_TEST_GLFWHELPER_HPP
