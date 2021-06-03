#ifndef URCHINENGINE_SHADERBUILDER_H
#define URCHINENGINE_SHADERBUILDER_H

#include <memory>
#include <map>
#include <vector>
#include <vulkan/vulkan.h>

#include <graphic/render/shader/model/Shader.h>

namespace urchin {

    class ShaderBuilder {
        public:
            static std::shared_ptr<Shader> createShader(const std::string&, const std::string&, const std::string&);
            static std::shared_ptr<Shader> createShader(const std::string&, const std::string&, const std::string&, std::unique_ptr<ShaderConstants>);

        private:
            static std::vector<char> readFile(const std::string&);
    };

}

#endif
