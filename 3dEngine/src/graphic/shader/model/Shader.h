#ifndef URCHINENGINE_SHADER_H
#define URCHINENGINE_SHADER_H

namespace urchin {

    class RenderTarget;

    class Shader {
        public:
            friend class ShaderVar;
            friend class RenderTarget;
            friend class ShaderDataSender;

            explicit Shader(unsigned int);
            ~Shader();

            unsigned int getShaderId() const;

        protected:
            void bind() const;

        private:
            unsigned int shaderId;
    };

}

#endif
