#ifndef URCHINENGINE_SHADER_H
#define URCHINENGINE_SHADER_H

namespace urchin {

    class OffscreenRender;
    class ScreenRender;

    class Shader {
        public:
            friend class ShaderVar;
            friend class ShaderDataSender;
            friend class OffscreenRender;
            friend class ScreenRender;

            explicit Shader(unsigned int);
            ~Shader();

        protected:
            unsigned int getShaderId() const;
            void bind() const;

        private:
            unsigned int shaderId;
    };

}

#endif
