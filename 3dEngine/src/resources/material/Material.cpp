#include "resources/material/Material.h"

namespace urchin
{
	
	Material::Material(Image *diffuseTexture, Image *normalTexture, float ambientFactor) :
			diffuseTexture(diffuseTexture),
			normalTexture(normalTexture),
			ambientFactor(ambientFactor)
	{
		if(diffuseTexture==nullptr)
		{
			auto *diffuseColorTab = new unsigned char[3]{255, 20, 147};
			this->diffuseTexture = new Image(3, 1, 1, Image::IMAGE_RGB, diffuseColorTab);
			this->diffuseTexture->toTexture(false, false);
		}

		if(normalTexture==nullptr)
		{
			auto *normalColorTab = new unsigned char[3]{127, 127, 255};
			this->normalTexture = new Image(3, 1, 1, Image::IMAGE_RGB, normalColorTab);
			this->normalTexture->toTexture(false, false);
		}
	}

	Material::~Material()
	{
		diffuseTexture->release();
		normalTexture->release();
	}

	const Image *Material::getDiffuseTexture() const
	{
		return diffuseTexture;
	}

	const Image *Material::getNormalTexture() const
	{
		return normalTexture;
	}

	float Material::getAmbientFactor() const
	{
		return ambientFactor;
	}

	std::vector<const Image *> Material::getTextures() const
	{
		std::vector<const Image *> textures = {getDiffuseTexture(), getNormalTexture()};

		return textures;
	}

}
