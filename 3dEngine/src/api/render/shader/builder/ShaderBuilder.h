#pragma once

#include <memory>
#include <map>
#include <vector>
#include <vulkan/vulkan.h>

#include <api/render/shader/model/Shader.h>

namespace urchin {

    class ShaderBuilder {
        public:
            static std::unique_ptr<Shader> createShader(const std::string&, const std::string&, const std::string&);
            static std::unique_ptr<Shader> createShader(const std::string&, const std::string&, const std::string&, std::unique_ptr<ShaderConstants>);

            static std::unique_ptr<Shader> createNullShader();

        private:
            static std::vector<char> readFile(const std::string&);
            static std::size_t computeShaderId(const std::string&, const std::string&, const std::string&, const ShaderConstants*);
    };

}
