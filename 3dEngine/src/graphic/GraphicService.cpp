#include <GL/glew.h>
#include "UrchinCommon.h"

#include "GraphicService.h"

namespace urchin {

    GraphicService::GraphicService() :
            errorDetected(false) {

    }

    void GraphicService::initializeGraphic() {
        //initialization Glew
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            throw std::runtime_error((char*)glewGetErrorString(err));
        }

        //check OpenGL version supported
        if (!glewIsSupported("GL_VERSION_4_5")) {
            throw std::runtime_error("OpenGL version 4.5 is required but it's not supported on this environment.");
        }

        //check OpenGL context version
        int majorVersionContext = 0, minorVersionContext = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &majorVersionContext);
        glGetIntegerv(GL_MINOR_VERSION, &minorVersionContext);

        if ((majorVersionContext*100 + minorVersionContext*10) < 450) {
            std::ostringstream ossMajorVersionContext;
            ossMajorVersionContext << majorVersionContext;

            std::ostringstream ossMinorVersionContext;
            ossMinorVersionContext << minorVersionContext;

            throw std::runtime_error("OpenGL context version required: 4.5 or more, current version: " + ossMajorVersionContext.str() + "." + ossMinorVersionContext.str() + ".");
        }

        //initialization OpenGl
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    void GraphicService::logErrors() {
        if(!errorDetected) {
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
                Logger::instance()->logError("OpenGL error detected: " + std::to_string(err));
            }
            errorDetected = true;
        }
    }

}
