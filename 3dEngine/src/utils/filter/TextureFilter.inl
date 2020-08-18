template<class BUILDER> TextureFilter::TextureFilter(const TextureFilterBuilder<BUILDER> *textureFilterBuilder):
    isInitialized(false),
    textureWidth(textureFilterBuilder->getTextureWidth()),
    textureHeight(textureFilterBuilder->getTextureHeight()),
    textureType(textureFilterBuilder->getTextureType()),
    textureAccessFilter(textureFilterBuilder->getTextureAccessFilter()),
    textureAnisotropy(textureFilterBuilder->getTextureAnisotropy()),
    textureNumberLayer(textureFilterBuilder->getTextureNumberLayer()),
    textureInternalFormat(textureFilterBuilder->getTextureInternalFormat()),
    textureFormat(textureFilterBuilder->getTextureFormat()),

    textureFilterShader(0),
    layersToUpdateLoc(0),
    fboID(0),
    textureID(0)
{

}