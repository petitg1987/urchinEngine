#pragma once

#include <memory>
#include <map>
#include <vector>

#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class ShaderBuilder {
        public:
            static std::unique_ptr<Shader> createShader(const std::string&, const std::string&);
            static std::unique_ptr<Shader> createShader(const std::string&, const std::string&, std::unique_ptr<ShaderConstants>);

            static std::unique_ptr<Shader> createComputeShader(const std::string&, std::unique_ptr<ShaderConstants>);

            static std::unique_ptr<Shader> createNullShader();

        private:
            static std::vector<char> readFile(const std::string&);
            static std::size_t computeShaderId(std::string_view, std::string_view, const ShaderConstants*);
    };

}
