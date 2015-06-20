#include <stdexcept>

#include "MapHandler.h"

namespace urchin
{

	/**
	 * Constructor which construct a new empty map
	 */
	MapHandler::MapHandler(SceneManager *sceneManager, PhysicsWorld *physicsWorld, SoundManager *soundManager) :
			sceneManager(sceneManager),
			physicsWorld(physicsWorld),
			soundManager(soundManager)
	{
		map = new Map(sceneManager, physicsWorld, soundManager);

		if(sceneManager==nullptr || !sceneManager->isInitialized())
		{
			throw std::invalid_argument("A scene manager initialized is expected.");
		}
	}

	/**
	 * Constructor which construct a map from a XML file
	 */
	MapHandler::MapHandler(SceneManager *sceneManager, PhysicsWorld *physicsWorld, SoundManager *soundManager, const std::string &filename) :
			MapHandler(sceneManager, physicsWorld, soundManager)
	{
		loadMapFromFile(filename);
	}

	MapHandler::~MapHandler()
	{
		delete map;
	}

	/**
	 * @param filename Absolute path to file containing map information
	 * @return Working directory relative to the map file
	 */
	std::string MapHandler::getRelativeWorkingDirectory(const std::string &filename)
	{
		XmlParser xmlParser(filename, "");
		return xmlParser.getRootChunk()->getAttributeValue(WORKING_DIR_ATTR);
	}

	/**
	 * @return Working directory relative to the map file
	 */
	std::string MapHandler::getRelativeWorkingDirectory() const
	{
		return relativeWorkingDirectory;
	}

	/**
	 * @param relativeWorkingDirectory Relative working directory to the map file
	 */
	void MapHandler::setRelativeWorkingDirectory(const std::string &relativeWorkingDirectory)
	{
		this->relativeWorkingDirectory = relativeWorkingDirectory;
	}

	void MapHandler::loadMapFromFile(const std::string &filename)
	{
		XmlParser xmlParser(filename);

		this->relativeWorkingDirectory = xmlParser.getRootChunk()->getAttributeValue(WORKING_DIR_ATTR);
		map->loadFrom(xmlParser.getRootChunk(), xmlParser);
	}

	void MapHandler::writeMapOnFile(const std::string &filename) const
	{
		XmlWriter xmlWriter(filename);

		std::shared_ptr<XmlChunk> rootChunk = xmlWriter.createChunk(SCENE_TAG, XmlAttribute(WORKING_DIR_ATTR, relativeWorkingDirectory));
		map->writeOn(rootChunk, xmlWriter);

		xmlWriter.saveInFile();
	}

	void MapHandler::refreshMap()
	{
		map->refreshMap();
	}

	Map *MapHandler::getMap() const
	{
		return map;
	}

}
