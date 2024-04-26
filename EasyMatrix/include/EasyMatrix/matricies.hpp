#pragma once
#include <stddef.h>
#include <EasyMatrix/vectors.hpp>
#include <EasyMatrix/BasicMath.hpp>
#include <math.h>

namespace EasyMatrix{

    template<typename T, size_t row_count, size_t column_count>
    struct Matrix{
    private:
        Vector<T, row_count> content[column_count] = {};
    
    public:
        size_t getColumnCount(){
            return column_count;
        }

        size_t getRowCount(){
            return row_count;
        }

        Vector<T, row_count>& operator [](long long i){
            return content[i];
        }
    };

    template<typename T, size_t s>
    Matrix<T, 1, s> vectorToMatrixRow(Vector<T, s> v){
        Matrix<T, 1, s> res;

        for(size_t i = 0; i < s; i++){
            res[i][0] = v[i];
        }
        
        return res;
    }

    template<typename T, size_t s>
    Matrix<T, s, 1> vectorToMatrixColumn(Vector<T, s> v){
        Matrix<T, s, 1> res;
        res[0] = v;
        return res;
    }

    template<typename T1, typename T2, size_t row_count, size_t column_count>
    Matrix<T1, row_count, column_count> operator + (Matrix<T1, row_count, column_count> op1, Matrix<T2, row_count, column_count> op2){
        Matrix<T1, row_count, column_count> result;
        for(size_t column = 0; column < column_count; column++){
            for(size_t row = 0; row < row_count; row++){
                result[column][row] = op1[column][row] + op2[column][row];
            }
        }
        return result;
    }

    template<typename T1, typename T2, size_t row_count, size_t column_count>
    Matrix<T1, row_count, column_count> operator - (Matrix<T1, row_count, column_count> op1, Matrix<T2, row_count, column_count> op2){
        Matrix<T1, row_count, column_count> result;
        for(size_t column = 0; column < column_count; column++){
            for(size_t row = 0; row < row_count; row++){
                result[column][row] = op1[column][row] - op2[column][row];
            }
        }
        return result;
    }

    template<typename T1, typename T2, size_t row_count, size_t column_count>
    Matrix<T1, row_count, column_count>& operator += (Matrix<T1, row_count, column_count>& op1, Matrix<T2, row_count, column_count> op2){
        op1 = op1 + op2;
        return op1;
    }

    template<typename T1, typename T2, size_t row_count, size_t column_count>
    Matrix<T1, row_count, column_count>& operator -= (Matrix<T1, row_count, column_count>& op1, Matrix<T2, row_count, column_count> op2){
        op1 = op1 - op2;
        return op1;
    }

    template<typename T, size_t n, size_t m, size_t k>
    Matrix<T, k, m> operator * (Matrix<T, n, m> op1, Matrix<T, k, n> op2){
        Matrix<T, k, m> res;

        for(size_t i = 0; i < k; i++){
            for(size_t j = 0; j < m; j++){
                for(size_t sc = 0; sc < n; sc++){
                    res[j][i] += op1[sc][i] * op2[j][sc];
                }
            }
        }

        return res;
    }

    typedef Matrix<float, 2,2> Matrix2x2f;
    typedef Matrix<float, 3,3> Matrix3x3f;
    typedef Matrix<float, 4,4> Matrix4x4f;

    typedef Matrix<int, 2,2> Matrix2x2i;
    typedef Matrix<int, 3,3> Matrix3x3i;
    typedef Matrix<int, 4,4> Matrix4x4i;

    inline Matrix4x4f createOrthographicProjection(float left, float right, float top, float bottom, float nearDist, float farDist){
        Matrix4x4f res{};

        res[0][0] = 2 / (right - left);
        res[1][1] = 2 / (top - bottom);
        res[2][2] = 2 / (farDist - nearDist);

        res[3][0] = -((right + left) / (right - left));
        res[3][1] = -((top + bottom) / (top - bottom));
        res[3][2] = -((farDist + nearDist) / (farDist-nearDist));
        res[3][3] = 1;

        return res;
    }

    inline Matrix4x4f createScaleMatrix(Vector3f scale){

        Matrix4x4f res{};

        res[0][0] = scale.x;
        res[1][1] = scale.y;
        res[2][2] = scale.z;
        res[3][3] = 1;

        return res;
    }

    inline Matrix4x4f createPositionMatrix(Vector3f position){
        Matrix4x4f res{};
        
        res[0][0] = 1;
        res[1][1] = 1;
        res[2][2] = 1;

        res[3][0] = position.x;
        res[3][1] = -position.y;
        res[3][2] = position.z;
        res[3][3] = 1;

        return res;
    }

    inline Matrix4x4f createRotationMatrixX(float angleRadians){
        Matrix4x4f res{};

        res[0][0] = 1;
        res[1][1] = cos(angleRadians);
        res[1][2] = sin(angleRadians);
        res[2][1] = -sin(angleRadians);
        res[2][2] = cos(angleRadians);
        res[3][3] = 1;

        return res;
    }

    inline Matrix4x4f createRotationMatrixY(float angleRadians){
        Matrix4x4f res{};

        res[0][0] = cos(angleRadians);
        res[0][2] = -sin(angleRadians);
        res[1][1] = 1;
        res[2][0] = sin(angleRadians);
        res[2][2] = cos(angleRadians);
        res[3][3] = 1;

        return res;
    }

    inline Matrix4x4f createRotationMatrixZ(float angleRadians){
        Matrix4x4f res{};

        res[0][0] = cos(angleRadians);
        res[0][1] = sin(angleRadians);
        res[1][0] = -sin(angleRadians);
        res[1][1] = cos(angleRadians);
        res[2][2] = 1;
        res[3][3] = 1;

        return res;
    }

    inline Matrix4x4f createRotationMatrix(Vector3f angleRadians){
        Matrix4x4f res{};
        res[0][0] = 1;
        res[1][1] = 1;
        res[2][2] = 1;
        res[3][3] = 1;

        if(angleRadians.x != 0){
            res = res * createRotationMatrixX(angleRadians.x);
        }
        if(angleRadians.y != 0){
            res = res * createRotationMatrixY(angleRadians.y);
        } 
        if(angleRadians.z != 0){
            res = res * createRotationMatrixZ(angleRadians.z);
        }

        return res;
    }

    inline Matrix4x4f createPerspectiveProjection(float fov, float width, float height, float nearDist, float farDist){
        Matrix4x4f res{};

        float aspect = std::min(width, height) / std::max(width, height);

        res[0][0] = (1 / tan(fov / 2)) / aspect;
        res[1][1] = 1 / tan(fov / 2);
        res[2][2] = (farDist + nearDist) / (farDist - nearDist);
        res[2][3] = 1;
        res[3][2] = -(2 * farDist * nearDist) / (farDist - nearDist);

        return res;
    }
};