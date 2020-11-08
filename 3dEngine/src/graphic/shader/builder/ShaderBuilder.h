#ifndef URCHINENGINE_SHADERBUILDER_H
#define URCHINENGINE_SHADERBUILDER_H

#include <string>
#include <list>
#include <map>
#include <memory>
#include "UrchinCommon.h"

#include "graphic/shader/builder/TokenReplacerShader.h"
#include "graphic/shader/model/Shader.h"

namespace urchin {

    class ShaderBuilder {
        public:
            std::unique_ptr<Shader> createShader(const std::string &, const std::string &, const std::string &);
            std::unique_ptr<Shader> createShader(const std::string &, const std::string &, const std::string &, const std::map<std::string, std::string> &);

        private:
            std::string readEntireFile(const std::string &);

            void checkShaderFile(unsigned int, const std::string &);
            void checkShader(unsigned int, const std::string &);
    };

}

#endif
