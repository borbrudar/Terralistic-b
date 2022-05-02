#include "graphics-internal.hpp"

void gfx::Transformation::applyMatrix(float applied_matrix[3][3]) {
    Transformation applied_transformation;
    memcpy(applied_transformation.matrix, applied_matrix, sizeof(matrix));
    *this = *this * applied_transformation;
}

void gfx::Transformation::stretch(float x, float y) {
    float applied_matrix[3][3] = {
        {x,   0.f, 0.f},
        {0.f, y,   0.f},
        {0.f, 0.f, 1.f},
    };
    applyMatrix(applied_matrix);
}

void gfx::Transformation::translate(float x, float y) {
    float applied_matrix[3][3] = {
        {1.f, 0.f, 0.f},
        {0.f, 1.f, 0.f},
        {x,   y,   1.f},
    };
    applyMatrix(applied_matrix);
}

float* gfx::Transformation::getArray() {
    return &matrix[0][0];
}

void gfx::Transformation::reset() {
    const float identity_matrix[3][3] = {
        {1.f, 0.f, 0.f},
        {0.f, 1.f, 0.f},
        {0.f, 0.f, 1.f},
    };
    memcpy(matrix, identity_matrix, sizeof(matrix));
}

gfx::Transformation gfx::Transformation::operator*(const Transformation& a) {
    Transformation result;
    for(int x = 0; x < 3; x++)
        for(int y = 0; y < 3; y++) {
            float sum = 0;
            for(int i = 0; i < 3; i++)
                sum += matrix[i][y] * a.matrix[x][i];
            result.matrix[x][y] = sum;
        }
    return result;
}
