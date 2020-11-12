#include <GL/glew.h>

#include "ShaderVar.h"

namespace urchin {

    ShaderVar::ShaderVar() :
            shader(nullptr),
            variableLocation(-1) {

    }

    ShaderVar::ShaderVar(const std::unique_ptr<Shader> &shader, std::string &&variableName) :
            shader(shader.get()) {
        shader->bind();
        variableLocation = glGetUniformLocation(shader->getShaderId(), variableName.c_str());
    }

    bool ShaderVar::isValid() const {
        return shader != nullptr && variableLocation != -1;
    }

    Shader *ShaderVar::getShader() const {
        return shader;
    }

    int ShaderVar::getVariableLocation() const {
        return variableLocation;
    }

}
