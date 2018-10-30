#include <locale>
#include <memory>
#include "UrchinCommon.h"

#include "loader/material/LoaderMTR.h"
#include "resources/MediaManager.h"
#include "resources/image/Image.h"

namespace urchin
{

	Material *LoaderMTR::loadFromFile(const std::string &filename, void *)
	{
		std::locale::global(std::locale("C")); //for float

		XmlParser parserXml(filename);

		//textures data
		bool needMipMaps = true;
		bool needAnisotropy = true;
		bool needRepeatTexture = false;
		std::shared_ptr<XmlChunk> repeatTexture(parserXml.getUniqueChunk(false, "repeatTexture"));
		if(repeatTexture!=nullptr)
		{
			needRepeatTexture = repeatTexture->getBoolValue();
		}

		//diffuse data
		Image *diffuseTex = nullptr;
		std::shared_ptr<XmlChunk> diffuse(parserXml.getUniqueChunk(false, "diffuse"));
		if(diffuse!=nullptr)
		{
			std::shared_ptr<XmlChunk> diffuseTexture(parserXml.getUniqueChunk(true, "texture", XmlAttribute(), diffuse));
			diffuseTex = MediaManager::instance()->getMedia<Image>(diffuseTexture->getStringValue());
			diffuseTex->toTexture(needMipMaps, needAnisotropy, needRepeatTexture);
		}

		//normal data
		Image *normalTex = nullptr;
		std::shared_ptr<XmlChunk> normal(parserXml.getUniqueChunk(false, "normal"));
		if(normal!=nullptr)
		{
			std::shared_ptr<XmlChunk> normalTexture(parserXml.getUniqueChunk(true, "texture", XmlAttribute(), normal));
			normalTex = MediaManager::instance()->getMedia<Image>(normalTexture->getStringValue());
			normalTex->toTexture(needMipMaps, needAnisotropy, needRepeatTexture);
		}

		//ambient data
		float fAmbientFactor = 0.0;
		std::shared_ptr<XmlChunk> ambient(parserXml.getUniqueChunk(false, "ambient"));
		if(ambient!=nullptr)
		{
			std::shared_ptr<XmlChunk> ambientFactor(parserXml.getUniqueChunk(true, "factor", XmlAttribute(), ambient));
			fAmbientFactor = Converter::toFloat(ambientFactor->getStringValue());
		}
		
		return new Material(diffuseTex, normalTex, fAmbientFactor);
	}

}
