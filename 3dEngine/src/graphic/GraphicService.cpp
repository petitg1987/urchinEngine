#include "UrchinCommon.h"

#include "libs/glad/glad.h"
#include "GraphicService.h"

namespace urchin {

    GraphicService::GraphicService() :
            errorDetected(false) {

    }

    void GraphicService::initializeGraphic() {
        constexpr int minVersionInt = 450;
        const std::string minVersionStr = "4.5";

        if(!gladLoadGL()) {
            throw std::runtime_error("Unable to initialize GLAD. Your system's graphic drivers must support OpenGL version " + minVersionStr);
        }

        //check OpenGL version loaded
        if((GLVersion.major * 100 + GLVersion.minor * 10) < minVersionInt) {
            throw std::runtime_error("OpenGL version " + minVersionStr + " is required. Loaded version: " + std::to_string(GLVersion.major) + "." + std::to_string(GLVersion.minor));
        }

        //check OpenGL context version
        int majorVersionContext = 0, minorVersionContext = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &majorVersionContext);
        glGetIntegerv(GL_MINOR_VERSION, &minorVersionContext);
        if ((majorVersionContext * 100 + minorVersionContext * 10) < minVersionInt) {
            throw std::runtime_error("OpenGL version " + minVersionStr + " is required. Context version: " + std::to_string(majorVersionContext) + "." + std::to_string(minorVersionContext));
        }

        //initialization OpenGl
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    void GraphicService::logErrors() {
        if (!errorDetected) {
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
                Logger::instance()->logError("OpenGL error detected: " + std::to_string(err));
            }
            errorDetected = true;
        }
    }

}
