//
// Created by Роман  Тимофеев on 20.04.2024.
//

#ifndef EASY_VULKAN_TEST_DEVICEFEATURES_HPP
#define EASY_VULKAN_TEST_DEVICEFEATURES_HPP
#include <cctype>

namespace EasyVK{
    enum FeaturesBits : uint32_t {
        COMPUTE = 0b1,
        GRAPHICS = 0b10,
        SWAP_CHAIN = 0b100,
    };

    typedef uint64_t Features;
}

#endif //EASY_VULKAN_TEST_DEVICEFEATURES_HPP
