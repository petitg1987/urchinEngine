#include <GL/glew.h>

#include "FogManager.h"
#include "utils/shader/ShaderManager.h"

namespace urchin
{
    FogManager::FogManager() :
            currentFog(nullptr),
            hasFogLoc(0),
            fogDensityLoc(0),
            fogGradientLoc(0),
            fogColorLoc(0),
            fogMaxHeightLoc(0)
    {

    }

    void FogManager::activateFog(Fog *fog)
    {
        currentFog = fog;

        loadFog();
    }

    void FogManager::disableFog()
    {
        currentFog = nullptr;

        loadFog();
    }

    bool FogManager::hasFog() const
    {
        return currentFog!=nullptr;
    }

    void FogManager::loadUniformLocationFor(unsigned int deferredShaderID)
    {
        this->deferredShaderID = deferredShaderID;

        ShaderManager::instance()->bind(deferredShaderID);
        hasFogLoc = glGetUniformLocation(deferredShaderID, "hasFog");
        fogDensityLoc = glGetUniformLocation(deferredShaderID, "fogDensity");
        fogGradientLoc = glGetUniformLocation(deferredShaderID, "fogGradient");
        fogColorLoc = glGetUniformLocation(deferredShaderID, "fogColor");
        fogMaxHeightLoc = glGetUniformLocation(deferredShaderID, "fogMaxHeight");

        loadFog();
    }

    void FogManager::loadFog()
    {
        ShaderManager::instance()->bind(deferredShaderID);
        glUniform1i(hasFogLoc, currentFog!=nullptr);

        if(currentFog!=nullptr)
        {
            glUniform1f(fogDensityLoc, currentFog->getDensity());
            glUniform1f(fogGradientLoc, currentFog->getGradient());
            glUniform4fv(fogColorLoc, 1, (const float *)Vector4<float>(currentFog->getColor(), 1.0));
            glUniform1f(fogMaxHeightLoc, currentFog->getMaxHeight());
        }
    }

}
