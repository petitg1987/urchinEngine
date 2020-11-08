#ifndef URCHINENGINE_SHADERDATASENDER_H
#define URCHINENGINE_SHADERDATASENDER_H

#include <memory>
#include <functional>

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"

namespace urchin {

    class ShaderDataSender {
        public:
            explicit ShaderDataSender(const std::unique_ptr<Shader> &);

            ShaderDataSender &sendData(const ShaderVar &, float);
            ShaderDataSender &sendData(const ShaderVar &, int);
            ShaderDataSender &sendData(const ShaderVar &, const Matrix4<float> &);
            ShaderDataSender &sendData(const ShaderVar &, const Vector3<float> &);

        private:
            const std::unique_ptr<Shader> &shader;
    };

}

#endif
