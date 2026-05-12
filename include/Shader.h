#ifndef SHADER_H
#define SHADER_H

#include <GL/gl.h>
#include <GL/glext.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setMat4(const std::string &name, const float* mat) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
