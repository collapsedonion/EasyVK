#pragma once

#define BASIC_MEMBERS VkResult _result = VK_NOT_READY;

#define BASIC_METHODS VkResult getResult(); bool isReady(); void destroy();

#define BASIC_METHOD__I(Class) VkResult EasyVK::Class::getResult(){return _result;} bool EasyVK::Class::isReady(){return _result == VK_SUCCESS;}

#define N_EVK namespace EasyVK

namespace EasyVK{
    class LogicalDevice;
}