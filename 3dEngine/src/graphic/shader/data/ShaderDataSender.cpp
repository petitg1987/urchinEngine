#include <GL/glew.h>
#include "UrchinCommon.h"

#include "ShaderDataSender.h"

namespace urchin {

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, float value) {
        shaderVar.getShader()->bind();
        glUniform1f(shaderVar.getVariableLocation(), value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, unsigned int count, const float* values) {
        shaderVar.getShader()->bind();
        glUniform1fv(shaderVar.getVariableLocation(), count, values);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, int value) {
        shaderVar.getShader()->bind();
        glUniform1i(shaderVar.getVariableLocation(), value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, unsigned int count, const int* values) {
        shaderVar.getShader()->bind();
        glUniform1iv(shaderVar.getVariableLocation(), count, values);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, unsigned int value) {
        shaderVar.getShader()->bind();
        glUniform1ui(shaderVar.getVariableLocation(), value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, unsigned int count, const unsigned int* values) {
        shaderVar.getShader()->bind();
        glUniform1uiv(shaderVar.getVariableLocation(), count, values);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, bool value) {
        return sendData(shaderVar, (int)value);
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, const Matrix2<float>& value) {
        shaderVar.getShader()->bind();
        glUniformMatrix2fv(shaderVar.getVariableLocation(), 1, GL_FALSE, (const float*)value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, unsigned int count, const Matrix2<float>* values) {
        shaderVar.getShader()->bind();
        glUniformMatrix2fv(shaderVar.getVariableLocation(), count, GL_FALSE, (const float*)values);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, const Matrix3<float>& value) {
        shaderVar.getShader()->bind();
        glUniformMatrix3fv(shaderVar.getVariableLocation(), 1, GL_FALSE, (const float*)value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, unsigned int count, const Matrix3<float>* values) {
        shaderVar.getShader()->bind();
        glUniformMatrix3fv(shaderVar.getVariableLocation(), count, GL_FALSE, (const float*)values);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, const Matrix4<float>& value) {
        shaderVar.getShader()->bind();
        glUniformMatrix4fv(shaderVar.getVariableLocation(), 1, GL_FALSE, (const float*)value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, unsigned int count, const Matrix4<float>* values) {
        shaderVar.getShader()->bind();
        glUniformMatrix4fv(shaderVar.getVariableLocation(), count, GL_FALSE, (const float*)values);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, const Point2<float>& value) {
        shaderVar.getShader()->bind();
        glUniform2fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, const Point3<float>& value) {
        shaderVar.getShader()->bind();
        glUniform3fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, const Point4<float>& value) {
        shaderVar.getShader()->bind();
        glUniform4fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, const Vector2<float>& value) {
        shaderVar.getShader()->bind();
        glUniform2fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, const Vector2<int>& value) {
        shaderVar.getShader()->bind();
        glUniform2iv(shaderVar.getVariableLocation(), 1, (const int*)value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, unsigned int count, const Vector2<float>* values) {
        shaderVar.getShader()->bind();
        glUniform2fv(shaderVar.getVariableLocation(), count, (const float*)values);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, const Vector3<float>& value) {
        shaderVar.getShader()->bind();
        glUniform3fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, unsigned int count, const Vector3<float>* values) {
        shaderVar.getShader()->bind();
        glUniform3fv(shaderVar.getVariableLocation(), count, (const float*)values);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, const Vector4<float>& value) {
        shaderVar.getShader()->bind();
        glUniform4fv(shaderVar.getVariableLocation(), 1, (const float*)value);
        return *this;
    }

    ShaderDataSender& ShaderDataSender::sendData(const ShaderVar& shaderVar, unsigned int count, const Vector4<float>* values) {
        shaderVar.getShader()->bind();
        glUniform4fv(shaderVar.getVariableLocation(), count, (const float*)values);
        return *this;
    }

}
