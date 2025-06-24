#include <cstring>

#include <scene/renderer3d/postprocess/antialiasing/ModelTaaShaderVariable.h>

namespace urchin {

    ModelTaaShaderVariable::ModelTaaShaderVariable() :
            CustomModelShaderVariable(),
            frameData({}),
            jitterData({}),
            jitterOffsetsUpdated(false) {
        std::memset((void *)&frameData, 0, sizeof(frameData));
        std::memset((void *)&jitterData, 0, sizeof(jitterData));

        frameData.frameCount = 0;

        haltonSequenceX.setValues(0.500000f, 0.250000f, 0.750000f, 0.125000f, 0.625000f, 0.375000f, 0.875000f, 0.062500f,
            0.562500f, 0.312500f, 0.812500f, 0.187500f, 0.687500f, 0.437500f, 0.937500f, 0.031250f);
        haltonSequenceY.setValues(0.333333f, 0.666667f, 0.111111f, 0.444444f, 0.777778f, 0.222222f, 0.555556f, 0.888889f,
            0.037037f, 0.370370f, 0.703704f, 0.148148f, 0.481481f, 0.814815f, 0.259259f, 0.592593f);
    }

    void ModelTaaShaderVariable::onRenderingSceneResize(unsigned int renderingSceneWidth, unsigned int renderingSceneHeight) {
        for (std::size_t i = 0; i < 16; ++i) {
            (&jitterData.offsetsX.a11)[i] = (((&haltonSequenceX.a11)[i] - 0.5f) * 2.0f) / (float)renderingSceneWidth;
            (&jitterData.offsetsY.a11)[i] = (((&haltonSequenceY.a11)[i] - 0.5f) * 2.0f) / (float)renderingSceneHeight;
        }
        jitterOffsetsUpdated = true;
    }

    void ModelTaaShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder, uint32_t uniformBinding1, uint32_t uniformBinding2) {
        assert(meshRendererBuilder->getUniformData().size() == 2);
        meshRendererBuilder->addUniformData(uniformBinding1, sizeof(frameData), &frameData);
        meshRendererBuilder->addUniformData(uniformBinding2, sizeof(jitterData), &jitterData);
    }

    void ModelTaaShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer, uint32_t uniformBinding1, uint32_t uniformBinding2) {
        frameData.frameCount++;
        meshRenderer.updateUniformData(uniformBinding1, &frameData);

        if (jitterOffsetsUpdated) {
            meshRenderer.updateUniformData(uniformBinding2, &jitterData);
            jitterOffsetsUpdated = false;
        }
    }

}
