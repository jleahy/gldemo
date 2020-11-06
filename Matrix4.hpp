#ifndef Matrix4_hpp
#define Matrix4_hpp


#include <iosfwd>


class Matrix4 {
public:
	static Matrix4 createProjectionMatrix(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
	static Matrix4 createViewMatrix(float cameraX, float cameraY, float cameraZ, float cameraPitch, float cameraYaw);
    static Matrix4 createTranslationMatrix(float x, float y, float z);
    static Matrix4 createRotationMatrix(float pitch, float yaw);

	Matrix4 operator*(const Matrix4 & rhs) const;
	Matrix4 & operator*=(const Matrix4 & rhs);

    const float * data() const { return _x; }

private:
    friend std::ostream & operator<<(std::ostream & os, const Matrix4 & rhs);

	float _x[16]; // row-major
} __attribute__((aligned(16)));


std::ostream & operator<<(std::ostream & os, const Matrix4 & rhs);


#endif
