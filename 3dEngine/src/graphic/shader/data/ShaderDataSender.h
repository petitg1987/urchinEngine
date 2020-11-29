#ifndef URCHINENGINE_SHADERDATASENDER_H
#define URCHINENGINE_SHADERDATASENDER_H

#include <memory>
#include <functional>

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"

namespace urchin {

    class ShaderDataSender {
        public:
            ShaderDataSender& sendData(const ShaderVar&, float);
            ShaderDataSender& sendData(const ShaderVar&, unsigned int, const float*);
            ShaderDataSender& sendData(const ShaderVar&, int);
            ShaderDataSender& sendData(const ShaderVar&, unsigned int, const int*);
            ShaderDataSender& sendData(const ShaderVar&, unsigned int);
            ShaderDataSender& sendData(const ShaderVar&, unsigned int, const unsigned int*);

            ShaderDataSender& sendData(const ShaderVar&, bool);

            ShaderDataSender& sendData(const ShaderVar&, const Matrix2<float>&);
            ShaderDataSender& sendData(const ShaderVar&, unsigned int, const Matrix2<float>*);
            ShaderDataSender& sendData(const ShaderVar&, const Matrix3<float>&);
            ShaderDataSender& sendData(const ShaderVar&, unsigned int, const Matrix3<float>*);
            ShaderDataSender& sendData(const ShaderVar&, const Matrix4<float>&);
            ShaderDataSender& sendData(const ShaderVar&, unsigned int, const Matrix4<float>*);

            ShaderDataSender& sendData(const ShaderVar&, const Point2<float>&);
            ShaderDataSender& sendData(const ShaderVar&, const Point3<float>&);
            ShaderDataSender& sendData(const ShaderVar&, const Point4<float>&);

            ShaderDataSender& sendData(const ShaderVar&, const Vector2<float>&);
            ShaderDataSender& sendData(const ShaderVar&, const Vector2<int>&);
            ShaderDataSender& sendData(const ShaderVar&, unsigned int, const Vector2<float>*);
            ShaderDataSender& sendData(const ShaderVar&, const Vector3<float>&);
            ShaderDataSender& sendData(const ShaderVar&, unsigned int, const Vector3<float>*);
            ShaderDataSender& sendData(const ShaderVar&, const Vector4<float>&);
            ShaderDataSender& sendData(const ShaderVar&, unsigned int, const Vector4<float>*);
    };

}

#endif
