#include <GL/glew.h>
#include "UrchinCommon.h"

#include "ShaderDataSender.h"

namespace urchin {

    ShaderDataSender::ShaderDataSender(const std::unique_ptr<Shader> &shader) :
            shader(shader.get()){

    }

    ShaderDataSender::ShaderDataSender(const std::shared_ptr<Shader> &shader) :
            shader(shader.get()){

    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, float value) {
        shader->bind();
        glUniform1f(shaderVar.getVariableLocation(), value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, int value) {
        shader->bind();
        glUniform1i(shaderVar.getVariableLocation(), value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, unsigned int value) {
        shader->bind();
        glUniform1ui(shaderVar.getVariableLocation(), value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, const Matrix2<float> &value) {
        shader->bind();
        glUniformMatrix2fv(shaderVar.getVariableLocation(), 1, GL_FALSE, (const float*)value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, const Matrix3<float> &value) {
        shader->bind();
        glUniformMatrix3fv(shaderVar.getVariableLocation(), 1, GL_FALSE, (const float*)value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, const Matrix4<float> &value) {
        shader->bind();
        glUniformMatrix4fv(shaderVar.getVariableLocation(), 1, GL_FALSE, (const float*)value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, const Point2<float> &value) {
        shader->bind();
        glUniform2fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, const Point3<float> &value) {
        shader->bind();
        glUniform3fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, const Point4<float> &value) {
        shader->bind();
        glUniform4fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, const Vector2<float> &value) {
        shader->bind();
        glUniform2fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, const Vector3<float> &value) {
        shader->bind();
        glUniform3fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender &ShaderDataSender::sendData(const ShaderVar &shaderVar, const Vector4<float> &value) {
        shader->bind();
        glUniform4fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

}
