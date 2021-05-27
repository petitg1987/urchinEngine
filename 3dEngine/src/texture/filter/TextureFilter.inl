template<class BUILDER> TextureFilter::TextureFilter(const TextureFilterBuilder<BUILDER>* textureFilterBuilder):
        isInitialized(false),
        name(textureFilterBuilder->getName()),
        sourceTexture(textureFilterBuilder->getSourceTexture()),
        textureWidth(textureFilterBuilder->getTextureWidth()),
        textureHeight(textureFilterBuilder->getTextureHeight()),
        textureType(textureFilterBuilder->getTextureType()),
        textureNumberLayer(textureFilterBuilder->getTextureNumberLayer()),
        textureFormat(textureFilterBuilder->getTextureFormat()) {
    if(textureNumberLayer > TEXTURE_LAYER_SHADER_LIMIT) {
        throw std::runtime_error("Texture layer value is limited to " + std::to_string(TEXTURE_LAYER_SHADER_LIMIT));
    }
}