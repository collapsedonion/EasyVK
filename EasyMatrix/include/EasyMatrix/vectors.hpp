#pragma once
#include <stddef.h> 
#include <math.h>

namespace EasyMatrix{
    template<typename T, size_t s>
    struct Vector
    {
    private:
        T content[s] = {};

    public:
        size_t getSize(){
            return s;
        } 

        T& operator [] (size_t id){
            return content[id];
        }
    };

    template<typename T>
    struct Vector<T, 2>{
    public:
        T u = 0;
        T v = 0;

    public:
        size_t getSize(){
            return 2;
        } 

        T& operator [] (size_t id){
            switch (id)
            {
            case 0:
                return u;
            case 1:
                return v;
            }
            return u;
        }
    };

    template<typename T>
    struct Vector<T, 3>{
    public:
        T x = 0;
        T y = 0;
        T z = 0;

    public:
        size_t getSize(){
            return 3;
        } 

        T& operator [] (size_t id){
            switch (id)
            {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            }
            return x;
        }
    };

    template<typename T>
    struct Vector<T, 4>{
    public:
        T x = 0;
        T y = 0;
        T z = 0;
        T w = 0;

    public:
        size_t getSize(){
            return 4;
        } 

        T& operator [] (size_t id){
            switch (id)
            {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
            }
            return x;
        }
    };

    template<typename T1, size_t s1, typename T2, size_t s2>
    Vector<T1, s1> operator + (Vector<T1,s1> op1, Vector<T2, s2> op2){
        size_t minSize = s2 > s1 ? s1 : s2;
        Vector<T1, s1> result;

        for(size_t i = 0; i < minSize; i++){
            result[i] = op1[i] + op2[i];
        }

        return result;
    }

    template<typename T1, size_t s1, typename T2, size_t s2>
    Vector<T1, s1> operator - (Vector<T1,s1> op1, Vector<T2, s2> op2){
        size_t minSize = s2 > s1 ? s1 : s2;
        Vector<T1, s1> result;

        for(size_t i = 0; i < minSize; i++){
            result[i] = op1[i] - op2[i];
        }

        return result;
    }

    template<typename T1, size_t s1, typename T2, size_t s2>
    Vector<T1, s1> operator * (Vector<T1,s1> op1, Vector<T2, s2> op2){
        size_t minSize = s2 > s1 ? s1 : s2;
        Vector<T1, s1> result;

        for(size_t i = 0; i < minSize; i++){
            result[i] = op1[i] * op2[i];
        }

        return result;
    }

    template<typename T1, size_t s1, typename T2, size_t s2>
    Vector<T1, s1> operator / (Vector<T1,s1> op1, Vector<T2, s2> op2){
        size_t minSize = s2 > s1 ? s1 : s2;
        Vector<T1, s1> result;

        for(size_t i = 0; i < minSize; i++){
            result[i] = op1[i] / op2[i];
        }

        return result;
    }
    
    template<typename T1, size_t s1, typename T2, size_t s2>
    Vector<T1, s1>& operator += (Vector<T1,s1>& op1, Vector<T2, s2> op2){
        op1 = op1 + op2;
        return op1;
    }

    template<typename T1, size_t s1, typename T2, size_t s2>
    Vector<T1, s1>& operator -= (Vector<T1,s1>& op1, Vector<T2, s2> op2){
        op1 = op1 - op2;
        return op1;
    }

    template<typename T1, size_t s1, typename T2, size_t s2>
    Vector<T1, s1>& operator *= (Vector<T1,s1>& op1, Vector<T2, s2> op2){
        op1 = op1 * op2;
        return op1;
    }

    template<typename T1, size_t s1, typename T2, size_t s2>
    Vector<T1, s1>& operator /= (Vector<T1,s1>& op1, Vector<T2, s2> op2){
        op1 = op1 / op2;
        return op1;
    }
    template<typename T, size_t s>
    Vector<T, s> operator * (Vector<T, s> op1, T op2){
        Vector<T, s> result;

        for(size_t i = 0; i < s; i++){
            result[i] = op1[i] * op2;
        }

        return result;
    }


    template<typename T, size_t s>
    Vector<T, s> operator / (Vector<T, s> op1, T op2){
        Vector<T, s> result;

        for(size_t i = 0; i < s; i++){
            result[i] = op1[i] / op2;
        }

        return result;
    }

    template<typename T, size_t s>
    Vector<T, s>& operator *= (Vector<T,s> & op1, T op2){
        op1 = op1 * op2;
        return op1;
    }

    template<typename T, size_t s>
    Vector<T, s>& operator /= (Vector<T,s> & op1, T op2){
        op1 = op1 / op2;
        return op1;
    }

    template<typename T, size_t s>
    float vectorLength(Vector<T, s> vector){
        float res = 0;
            
        for(size_t i = 0; i < vector.getSize(); i++){
            res += pow(vector[i], 2);
        }

        res = sqrt(res);
        return res;
    }

    template<typename T, size_t s>
    Vector<T, s> vectorNormalise(Vector<T, s> vector){
        return vector / vectorLength(vector);
    }

    typedef Vector<float, 2> Vector2f;
    typedef Vector<float, 3> Vector3f;
    typedef Vector<float, 4> Vector4f;

    typedef Vector<int, 2> Vector2i;
    typedef Vector<int, 3> Vector3i;
    typedef Vector<int, 4> Vector4i;
}