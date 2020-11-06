
#include <iostream>
#include <cmath>
#include <emmintrin.h>

#include "Matrix4.hpp"


Matrix4 Matrix4::createProjectionMatrix(float fieldOfView, float aspectRatio, float nearPlane, float farPlane) {
	float focalLength = 1.0f / tanf(fieldOfView * float(0.5 * 2.0 * M_PI / 360.0));
	float invDepth = 1.0f / (nearPlane - farPlane);

	Matrix4 ret;
	_mm_store_ps(&ret._x[0], _mm_setr_ps(focalLength / aspectRatio, 0.0f, 0.0f, 0.0f));
	_mm_store_ps(&ret._x[4], _mm_setr_ps(0.0f, focalLength, 0.0f, 0.0f));
	_mm_store_ps(&ret._x[8], _mm_setr_ps(0.0f, 0.0f, (farPlane + nearPlane) * invDepth, 2.0f * nearPlane * farPlane * invDepth));
	_mm_store_ps(&ret._x[12], _mm_setr_ps(0.0f, 0.0f, -1.0f, 0.0f));
	return ret;
}


Matrix4 Matrix4::createViewMatrix(float cameraX, float cameraY, float cameraZ, float cameraPitch, float cameraYaw) {
	float yawSin = sinf(cameraYaw);
	float yawCos = cosf(cameraYaw);
	float pitchSin = sinf(cameraPitch);
	float pitchCos = cosf(cameraPitch);

	Matrix4 ret;
	_mm_store_ps(&ret._x[0], _mm_setr_ps(yawCos,		    0,	                -yawSin,	        0.0f));
	_mm_store_ps(&ret._x[4], _mm_setr_ps(yawSin*pitchSin,	pitchCos,			pitchSin*yawCos,	0.0f));
	_mm_store_ps(&ret._x[8], _mm_setr_ps(yawSin*pitchCos,	-pitchSin,	        yawCos*pitchCos,	0.0f));
	_mm_store_ps(&ret._x[12], _mm_setr_ps(0.0f,             0.0f,               0.0f,               1.0f));

	return ret * createTranslationMatrix(-cameraX, -cameraY, -cameraZ);
}


Matrix4 Matrix4::createTranslationMatrix(float x, float y, float z) {
    Matrix4 ret;
    _mm_store_ps(&ret._x[0], _mm_setr_ps(1.0f, 0.0f, 0.0f, x));
    _mm_store_ps(&ret._x[4], _mm_setr_ps(0.0f, 1.0f, 0.0f, y));
    _mm_store_ps(&ret._x[8], _mm_setr_ps(0.0f, 0.0f, 1.0f, z));
    _mm_store_ps(&ret._x[12], _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f));
    return ret;
}


Matrix4 Matrix4::operator*(const Matrix4 & rhs) const {
	Matrix4 ret;
    __m128 row1 = _mm_load_ps(&rhs._x[0]);
	__m128 row2 = _mm_load_ps(&rhs._x[4]);
	__m128 row3 = _mm_load_ps(&rhs._x[8]);
	__m128 row4 = _mm_load_ps(&rhs._x[12]);
    for (int i=0; i<4; i++) {
        __m128 brod1 = _mm_set1_ps(_x[4*i + 0]);
        __m128 brod2 = _mm_set1_ps(_x[4*i + 1]);
        __m128 brod3 = _mm_set1_ps(_x[4*i + 2]);
        __m128 brod4 = _mm_set1_ps(_x[4*i + 3]);
        __m128 row = _mm_add_ps(_mm_add_ps(_mm_mul_ps(brod1, row1), _mm_mul_ps(brod2, row2)),
								_mm_add_ps(_mm_mul_ps(brod3, row3), _mm_mul_ps(brod4, row4)));
        _mm_store_ps(&ret._x[4*i], row);
    }
	return ret;
}


Matrix4 & Matrix4::operator*=(const Matrix4 & rhs) {
    __m128 row1 = _mm_load_ps(&rhs._x[0]);
	__m128 row2 = _mm_load_ps(&rhs._x[4]);
	__m128 row3 = _mm_load_ps(&rhs._x[8]);
	__m128 row4 = _mm_load_ps(&rhs._x[12]);
    for (int i=0; i<4; i++) {
        __m128 brod1 = _mm_set1_ps(_x[4*i + 0]);
        __m128 brod2 = _mm_set1_ps(_x[4*i + 1]);
        __m128 brod3 = _mm_set1_ps(_x[4*i + 2]);
        __m128 brod4 = _mm_set1_ps(_x[4*i + 3]);
        __m128 row = _mm_add_ps(_mm_add_ps(_mm_mul_ps(brod1, row1), _mm_mul_ps(brod2, row2)),
								_mm_add_ps(_mm_mul_ps(brod3, row3), _mm_mul_ps(brod4, row4)));
        _mm_store_ps(&_x[4*i], row);
    }
	return *this;
}


std::ostream & operator<<(std::ostream & os, const Matrix4 & rhs) {
    os << "{\n";
    for (int i=0; i<4; ++i) {
        os << rhs._x[4*i] << ", " << rhs._x[4*i+1] << ", " << rhs._x[4*i+2] << ", " << rhs._x[4*i+3] << "\n";
    }
    os << "}";
    return os;
}


