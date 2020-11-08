#include <GL/glew.h>

#include "ShaderVariable.h"

namespace urchin {

    ShaderVariable::ShaderVariable(const std::unique_ptr<Shader> &shader, std::string &variableName) {
        shader->bind();
        variableLocation = glGetUniformLocation(shader->getShaderId(), variableName.c_str());
    }

    int ShaderVariable::getVariableLocation() const {
        return variableLocation;
    }

}
