#ifndef GLApp_hpp
#define GLApp_hpp

#include "Matrix4.hpp"
#include "BufferAllocator.hpp"
#include "Shader.hpp"


class GLApp {
public:
    static const double PHYSICS_RESOLUTION;
    static const float MOVEMENT_SPEED;
    static const float LOOK_SPEED;

    GLApp();

    void resize(int w, int h);
    void onKey(char key, bool pressed);
    void onMouse(int dx, int dy);

    void render();
    void update();

private:
    void updateMatrices();

    enum {
        KEY_W = 1,
        KEY_A = 2,
        KEY_S = 4,
        KEY_D = 8,
    };

    Matrix4         _transformMatrix;

    // Vertex format is x,y,z as signed shorts
    BufferAllocator     _vertexBuffer;
    BufferAllocator     _indexBuffer;

    BufferAllocator::Ref    _vertexRef;
    BufferAllocator::Ref    _indexRef;

    ShaderProgram       _mainShader;
    unsigned            _vertexArray;
    unsigned            _projectionViewMatrixLoc;

    float           _cameraX;
    float           _cameraY;
    float           _cameraZ;
    float           _cameraPitch;
    float           _cameraYaw;
    float           _aspectRatio;
    uint32_t        _keysDown;
};


#endif
