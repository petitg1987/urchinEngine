template<class BUILDER> TextureFilter::TextureFilter(const TextureFilterBuilder<BUILDER>* textureFilterBuilder):
        isInitialized(false),
        isTestMode(textureFilterBuilder->isTestMode()),
        name(textureFilterBuilder->getName()),
        sourceTexture(textureFilterBuilder->getSourceTexture()),
        textureWidth(textureFilterBuilder->getTextureWidth()),
        textureHeight(textureFilterBuilder->getTextureHeight()),
        textureType(textureFilterBuilder->getTextureType()),
        textureFormat(textureFilterBuilder->getTextureFormat()) {

}