
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLApp.hpp"


const double GLApp::PHYSICS_RESOLUTION = 25e-3;
const float GLApp::MOVEMENT_SPEED = 0.1f;
const float GLApp::LOOK_SPEED = 0.01f;


GLApp::GLApp() :
    _cameraX(0.0f),
    _cameraY(0.0f),
    _cameraZ(0.0f),
    _cameraPitch(0.0f),
    _cameraYaw(0.0f),
    _keysDown(0) {

    // Set basic opengl properties
    glClearColor(0.4f, 0.6f, 0.9f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(65535);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Link sharer
    Shader vertexShader(GL_VERTEX_SHADER);
    vertexShader.load(
"#version 330 core\n"
"#line " S__LINE__ "\n"
"in vec3 vertexPosition;\n"
"uniform mat4 projectionViewMatrix;\n"
"void main() {\n"
"   gl_Position = projectionViewMatrix * vec4(vertexPosition, 1.0);\n"
"}\n"
    );
    Shader fragmentShader(GL_FRAGMENT_SHADER);
    fragmentShader.load(
"#version 330 core\n"
"#line " S__LINE__ "\n"
"out vec4 color;\n"
"void main() {\n"
"   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n"
    );
    _mainShader.attach(std::move(vertexShader));
    _mainShader.attach(std::move(fragmentShader));
    _mainShader.link();
    _projectionViewMatrixLoc = _mainShader.getUniformLoc("projectionViewMatrix");

    // Allocate buffers
    _vertexBuffer = BufferAllocator(1024, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    _indexBuffer = BufferAllocator(1024, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);

    // Load some data
    static const float data[] = {
        -0.5f, -0.5f, -3.5f,
        0.5f, -0.5f, -3.5f,
        0.0f, 0.5f, -3.5f,
    };
    _vertexRef = _vertexBuffer.allocate((void*)data, sizeof(data));
    static const int indices[] = {
        0, 1, 2
    };
    _indexRef = _indexBuffer.allocate((void*)indices, sizeof(indices));

    // Setup VAO
    glGenVertexArrays(1, &_vertexArray);
    glBindVertexArray(_vertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, *_vertexBuffer);
    glVertexAttribPointer(_mainShader.getAttributeLoc("vertexPosition"), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *_indexBuffer);

    glBindVertexArray(0);
}


void GLApp::resize(int w, int h) {
    // Set viewport
    glViewport(0, 0, w, h);

    // Save aspect ratio
    _aspectRatio = float(w) / float(h);
}


void GLApp::updateMatrices() {
    // Create projection matrix
    float fieldOfView = 60.0f;
    float nearPlane = 1.0f;
    float farPlane = 100.0f;
    Matrix4 projectionMatrix = Matrix4::createProjectionMatrix(fieldOfView, _aspectRatio, nearPlane, farPlane);

    // Create view matrix
    Matrix4 viewMatrix = Matrix4::createViewMatrix(_cameraX, _cameraY, _cameraZ, _cameraPitch, _cameraYaw);

    // Save composite transformation
    _transformMatrix = projectionMatrix * viewMatrix;
}


void GLApp::render() {
    // Setup matrix
    updateMatrices();

    // Clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup shader
    glUseProgram(*_mainShader);
    glUniformMatrix4fv(_projectionViewMatrixLoc, 1, GL_TRUE, _transformMatrix.data());

    // Indexed draw
    glBindVertexArray(_vertexArray);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Disable shader
    glUseProgram(0);
}


void GLApp::onKey(char key, bool pressed) {
    uint32_t mask;
    switch (key) {
    case 'W':
        mask = KEY_W;
        break;
    case 'A':
        mask = KEY_A;
        break;
    case 'S':
        mask = KEY_S;
        break;
    case 'D':
        mask = KEY_D;
        break;
    default:
        return;
    }
    if (pressed) {
        _keysDown |= mask;
    } else {
        _keysDown &= ~mask;
    }
}


void GLApp::onMouse(int dx, int dy) {
    _cameraPitch -= LOOK_SPEED * dy;
    _cameraPitch = std::min(_cameraPitch, 1.0f);
    _cameraPitch = std::max(_cameraPitch, -1.0f);
    _cameraYaw -= LOOK_SPEED * dx;
}


void GLApp::update() {
    if (_keysDown & KEY_W) {
        _cameraZ -= MOVEMENT_SPEED;
    }
    if (_keysDown & KEY_A) {
        _cameraX -= MOVEMENT_SPEED;
    }
    if (_keysDown & KEY_S) {
        _cameraZ += MOVEMENT_SPEED;
    }
    if (_keysDown & KEY_D) {
        _cameraX += MOVEMENT_SPEED;
    }
}



