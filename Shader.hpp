#ifndef Shader_hpp
#define Shader_hpp

#include <vector>
#include <string>

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)


class Shader {
public:
	Shader(GLenum shaderType);
    Shader(Shader && rhs);
    Shader & operator=(Shader && rhs);
	~Shader();

	void load(const char * code);

    int operator*() {
        return _shader;
    }

private:
	unsigned _shader;
};


class ShaderProgram {
public:
	ShaderProgram();
    ShaderProgram(ShaderProgram && rhs);
    ShaderProgram & operator=(ShaderProgram && rhs);
	~ShaderProgram();

    void attach(Shader && shader);
    void link();

    int getUniformLoc(const std::string & name);
    int getAttributeLoc(const std::string & name);

	int operator*() {
        return _shaderProgram;
    }

private:
    std::vector<Shader> _shaders;
    unsigned _shaderProgram;
};


#endif
