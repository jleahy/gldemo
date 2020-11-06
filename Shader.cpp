
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <cstring>

#include "Shader.hpp"


Shader::Shader(GLenum shaderType) :
	_shader(glCreateShader(shaderType)) {
	if (!_shader) {
		throw std::runtime_error("Could not create shader");
	}
}


Shader::Shader(Shader && rhs) :
    _shader(0) {
    std::swap(_shader, rhs._shader);
}


Shader & Shader::operator=(Shader && rhs) {
    std::swap(_shader, rhs._shader);
    return *this;
}


Shader::~Shader() {
    glDeleteShader(_shader);
}


void Shader::load(const char * code) {
	int len = strlen(code);
	glShaderSource(_shader, 1, &code, &len);
	glCompileShader(_shader);
	int status;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char info[1024];
		int infoLen;
		glGetShaderInfoLog(_shader, 1024, &infoLen, info);
		throw std::runtime_error("Error creating shader:\n" + std::string(info, infoLen));
	}
}


ShaderProgram::ShaderProgram() :
    _shaderProgram(glCreateProgram()) {
	if (!_shaderProgram) {
		throw std::runtime_error("Could not create shader program");
	}
}


ShaderProgram::ShaderProgram(ShaderProgram && rhs) :
    _shaderProgram(0) {
    std::swap(_shaderProgram, rhs._shaderProgram);
}


ShaderProgram & ShaderProgram::operator=(ShaderProgram && rhs) {
    std::swap(_shaderProgram, rhs._shaderProgram);
    return *this;
}


ShaderProgram::~ShaderProgram() {
    glDeleteProgram(_shaderProgram);
}


void ShaderProgram::attach(Shader && shader) {
    _shaders.push_back(std::move(shader));
    glAttachShader(_shaderProgram, *_shaders.back());
}


void ShaderProgram::link() {
    glLinkProgram(_shaderProgram);
	int status;
	glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &status);
	if (!status) {
		char info[1024];
		int infoLen;
		glGetProgramInfoLog(_shaderProgram, 1024, &infoLen, info);
		throw std::runtime_error("Error linking shader: " + std::string(info, infoLen));
	}
}


int ShaderProgram::getUniformLoc(const std::string & name) {
    return glGetUniformLocation(_shaderProgram, name.c_str());
}


int ShaderProgram::getAttributeLoc(const std::string & name) {
    return glGetAttribLocation(_shaderProgram, name.c_str());
}

