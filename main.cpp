
#include <iostream>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLApp.hpp"


namespace {


std::unique_ptr<GLApp> app;


bool hasMouseMoved = false;
double mouseX, mouseY;


void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        app->onKey(key, true);
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    } else if (action == GLFW_RELEASE) {
        app->onKey(key, false);
    }
}


void cursor_pos_callback(GLFWwindow * window, double xpos, double ypos) {
    if (!hasMouseMoved) {
        hasMouseMoved = true;
        mouseX = xpos;
        mouseY = ypos;
    }
    app->onMouse(xpos - mouseX, ypos - mouseY);
    mouseX = xpos;
    mouseY = ypos;
}


void framebuffer_size_callback(GLFWwindow * window, int width, int height) {
    app->resize(width, height);
}


}


int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        return 1;
    }

    try {
        // Create window and opengl context
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
        GLFWwindow * window = glfwCreateWindow(800, 600, "Render", NULL, NULL);
        if (!window) {
            std::cerr << "Error: failed to create window" << std::endl;
            glfwTerminate();
            return 1;
        }
        glfwSetWindowTitle(window, "Render");

        // Move window to center of primary monitor
        GLFWmonitor * primary = glfwGetPrimaryMonitor();
        int monitorX, monitorY;
        glfwGetMonitorPos(primary, &monitorX, &monitorY);
        const GLFWvidmode * mode = glfwGetVideoMode(primary);
        int newX = monitorX + (mode->width >> 1) - (800/2);
        int newY = monitorY + (mode->height >> 1) - (600/2);
        glfwSetWindowPos(window, newX, newY);

        // Setup opengl
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
            glfwTerminate();
            return 1;
        }
        glGetError(); // glew is odd an leaves behind an error

        // Setup app
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        app = std::unique_ptr<GLApp>(new GLApp());
        app->resize(width, height);
        glfwSetFramebufferSizeCallback(window, &framebuffer_size_callback);
        glfwSetCursorPosCallback(window, &cursor_pos_callback);
        glfwSetKeyCallback(window, &key_callback);

        // Show the window
        glfwShowWindow(window);

        // Loop until the user closes the window
        double now = glfwGetTime();
        double lag = 0.0;
        while (!glfwWindowShouldClose(window)) {
            // Find how much time has elapsed since previous render
            double newTs = glfwGetTime();
            double dt = newTs - now;
            now = newTs;

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);

            // Run physics steps
            lag += dt;
            while (lag >= GLApp::PHYSICS_RESOLUTION) {
                app->update();
                lag -= GLApp::PHYSICS_RESOLUTION;
            }

            // Render
            // TODO should get a new timestamp and provide a lerp
            app->render();

            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();

            // Check for errors
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << err << std::endl;
            }
        }

        glfwTerminate();
    } catch (const std::exception & e) {
        std::cerr << "Error: " << e.what() << std::endl;
        glfwTerminate();
        return 1;
    }
    return 0;
}

