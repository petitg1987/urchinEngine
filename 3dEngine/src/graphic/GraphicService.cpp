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
        const std::string recommendation = "Your graphic drivers must be updated to support OpenGL version " + minVersionStr;

        if(!gladLoadGL()) {
            throw UserAuthorityException("Unable to initialize GLAD", recommendation);
        }

        //check OpenGL version loaded
        if((GLVersion.major * 100 + GLVersion.minor * 10) < minVersionInt) {
            throw UserAuthorityException("OpenGL version " + minVersionStr + " is required but loaded version is: " + std::to_string(GLVersion.major)
                + "." + std::to_string(GLVersion.minor), recommendation);
        }

        //check OpenGL context version
        int majorVersionContext = 0, minorVersionContext = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &majorVersionContext);
        glGetIntegerv(GL_MINOR_VERSION, &minorVersionContext);
        if ((majorVersionContext * 100 + minorVersionContext * 10) < minVersionInt) {
            throw UserAuthorityException("OpenGL version " + minVersionStr + " is required but context version is: " + std::to_string(majorVersionContext)
                + "." + std::to_string(minorVersionContext), recommendation);
        }

        //initialization OpenGl
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
