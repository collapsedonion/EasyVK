//
// Created by Роман  Тимофеев on 25.04.2024.
//
#include <EasyVulkan/Base.hpp>

EasyVK::AutoFree::AutoFree() {
    this->referenceCounter = new int();
    *referenceCounter = 1;
}

EasyVK::AutoFree::AutoFree(const EasyVK::AutoFree &ref) {
    this->referenceCounter = ref.referenceCounter;
    (*this->referenceCounter)++;
}

EasyVK::AutoFree::~AutoFree() {
    (*referenceCounter)--;
    if(*referenceCounter == 0) {
        delete this->referenceCounter;
    }
}

bool EasyVK::AutoFree::isKilled() {
    return (*this->referenceCounter) == 1;
}

EasyVK::AutoFree &EasyVK::AutoFree::operator = (const EasyVK::AutoFree &other) {
    if(this != &other){
        *other.referenceCounter += 1;
        *this->referenceCounter -= 1;
        this->referenceCounter = other.referenceCounter;
    }

    return *this;
}

EasyVK::AutoFree &EasyVK::AutoFree::operator = (EasyVK::AutoFree &&other)  noexcept {
    --*this->referenceCounter;
    this->referenceCounter = other.referenceCounter;

    other.referenceCounter = nullptr;

    return *this;
}

EasyVK::AutoFree::AutoFree(EasyVK::AutoFree &&ref) noexcept{
    (*this->referenceCounter)--;
    this->referenceCounter = ref.referenceCounter;
    ref.referenceCounter = nullptr;
}
