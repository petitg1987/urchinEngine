#include <GL/glew.h>
#include <cassert>

#include "Shader.h"

namespace urchin {

    Shader::Shader(unsigned int shaderId) :
            shaderId(shaderId) {
        assert(shaderId != 0);
    }

    Shader::~Shader() {
        if (shaderId == 0) {
            return;
        }

        int countAttachedShaders = 0;
        int maxCount = 10;
        auto* attachedShaders = new unsigned int[(unsigned long)maxCount];

        glGetAttachedShaders(shaderId, maxCount, &countAttachedShaders, attachedShaders);

        for (int i=0; i<countAttachedShaders; ++i) {
            glDetachShader(shaderId, attachedShaders[i]);
            glDeleteShader(attachedShaders[i]);
        }

        delete[] attachedShaders;
        glDeleteProgram(shaderId);
    }

    unsigned int Shader::getShaderId() const {
        return shaderId;
    }

    void Shader::bind() const {
        glUseProgram(shaderId);
    }
}
