template<class BUILDER> TextureFilter::TextureFilter(const TextureFilterBuilder<BUILDER>* textureFilterBuilder):
    isInitialized(false),
    textureWidth(textureFilterBuilder->getTextureWidth()),
    textureHeight(textureFilterBuilder->getTextureHeight()),
    textureType(textureFilterBuilder->getTextureType()),
    textureNumberLayer(textureFilterBuilder->getTextureNumberLayer()),
    textureFormat(textureFilterBuilder->getTextureFormat()) {

}