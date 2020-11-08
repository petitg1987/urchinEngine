#include <GL/glew.h>

#include "ShaderVar.h"

namespace urchin {

    ShaderVar::ShaderVar() :
            variableLocation(-1) {

    }

    ShaderVar::ShaderVar(const std::unique_ptr<Shader> &shader, std::string &&variableName) {
        shader->bind();
        variableLocation = glGetUniformLocation(shader->getShaderId(), variableName.c_str());
    }

    ShaderVar::ShaderVar(const std::shared_ptr<Shader> &shader, std::string &&variableName) {
        shader->bind();
        variableLocation = glGetUniformLocation(shader->getShaderId(), variableName.c_str());
    }

    int ShaderVar::getVariableLocation() const {
        return variableLocation;
    }

}
