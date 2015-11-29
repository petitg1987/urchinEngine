#include <locale>
#include <stdexcept>
#include <memory>
#include "UrchinCommon.h"

#include "loader/material/LoaderMTR.h"
#include "resources/MediaManager.h"
#include "resources/image/Image.h"

namespace urchin
{
	
	LoaderMTR::~LoaderMTR()
	{

	}

	Material *LoaderMTR::loadFromFile(const std::string &filename, void *params)
	{
		std::locale::global(std::locale("C")); //for float

		XmlParser parserXml(filename);
		
		//diffuse data
		Image *diffuseTex = nullptr;
		std::shared_ptr<XmlChunk> diffuse(parserXml.getUniqueChunk(false, "diffuse"));
		if(diffuse.get()!=nullptr)
		{
			std::shared_ptr<XmlChunk> diffuseTexture(parserXml.getUniqueChunk(true, "texture", XmlAttribute(), diffuse));
			diffuseTex = MediaManager::instance()->getMedia<Image>(diffuseTexture->getStringValue());
			diffuseTex->toTexture(true, true);
		}

		//normal data
		Image *normalTex = nullptr;
		std::shared_ptr<XmlChunk> normal(parserXml.getUniqueChunk(false, "normal"));
		if(normal.get()!=nullptr)
		{
			std::shared_ptr<XmlChunk> normalTexture(parserXml.getUniqueChunk(true, "texture", XmlAttribute(), normal));
			normalTex = MediaManager::instance()->getMedia<Image>(normalTexture->getStringValue());
			normalTex->toTexture(true, false);
		}

		//ambient data
		float fAmbientFactor = 0.0;
		std::shared_ptr<XmlChunk> ambient(parserXml.getUniqueChunk(false, "ambient"));
		if(ambient.get()!=nullptr)
		{
			std::shared_ptr<XmlChunk> ambientFactor(parserXml.getUniqueChunk(true, "factor", XmlAttribute(), ambient));
			fAmbientFactor = Converter::toFloat(ambientFactor->getStringValue());
		}
		
		return new Material(diffuseTex, normalTex, fAmbientFactor);
	}

}
