#pragma once
#include <EasyMatrix/vectors.hpp>
#include <EasyMatrix/matricies.hpp>

namespace EasyMatrix{
    struct Transform{
        EasyMatrix::Vector3f position = {0,0,0};
        EasyMatrix::Vector3f rotation = {0,0,0};
        EasyMatrix::Vector3f scale = {1,1,1};

    inline EasyMatrix::Matrix4x4f toMatrix(){
        return
        EasyMatrix::createPositionMatrix(position) *  
        EasyMatrix::createRotationMatrix(rotation) *
        EasyMatrix::createScaleMatrix(scale);
    }
    };
};