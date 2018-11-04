#include <GL/glew.h>
#include <fstream>
#include <stdexcept>

#include "utils/shader/ShaderManager.h"

namespace urchin
{

	ShaderManager::ShaderManager() : Singleton<ShaderManager>(),
			shadersDirectoryName(ConfigService::instance()->getStringValue("shaders.shadersLocation")),
			currentProgramID(0)
	{
		this->shadersParentDirectory = FileSystem::instance()->getResourcesDirectory();
	}

	ShaderManager::~ShaderManager()
	{
		for(std::list<unsigned int>::const_iterator it = programs.begin(); it!=programs.end(); ++it)
		{
			clearProgram(*it);
		}
	}

	void ShaderManager::replaceShadersParentDirectoryBy(const std::string &shadersParentDirectory)
	{
		this->shadersParentDirectory = shadersParentDirectory;
	}

	unsigned int ShaderManager::createProgram(const std::string &vertexShaderFilename, const std::string &geometryShaderFilename,
			const std::string &fragmentShaderFilename)
	{
		std::map<std::string, std::string> emptyTokens;
		return createProgram(vertexShaderFilename, geometryShaderFilename, fragmentShaderFilename, emptyTokens);
	}

	unsigned int ShaderManager::createProgram(const std::string &vertexShaderFilename, const std::string &geometryShaderFilename,
			const std::string &fragmentShaderFilename, const std::map<std::string, std::string> &tokens)
	{
		unsigned int programID = glCreateProgram();
		programs.push_back(programID);

		//vertex shader
		const std::string &vertexShaderFileSource = readEntireFile(shadersParentDirectory + shadersDirectoryName + vertexShaderFilename);
		const std::string &vertexShaderSourceStep1 = tokenReplacerShader.replaceTokens(vertexShaderFileSource, tokens);
		const std::string &vertexShaderSource = loopUnrollerShader.unrollLoops(vertexShaderSourceStep1);
		const char *vertexShaderSourceChar = vertexShaderSource.c_str();
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSourceChar, nullptr);
		glCompileShader(vertexShader);
		checkShader(vertexShader, vertexShaderFilename);
		glAttachShader(programID, vertexShader);

		//geometry shader
		if(!geometryShaderFilename.empty())
		{
			const std::string &geometryShaderFileSource = readEntireFile(shadersParentDirectory + shadersDirectoryName + geometryShaderFilename);
			const std::string &geometryShaderSourceStep1 = tokenReplacerShader.replaceTokens(geometryShaderFileSource, tokens);
			const std::string &geometryShaderSource = loopUnrollerShader.unrollLoops(geometryShaderSourceStep1);
			const char *geometryShaderSourceChar = geometryShaderSource.c_str();
			unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

			glShaderSource(geometryShader, 1, &geometryShaderSourceChar, nullptr);
			glCompileShader(geometryShader);
			checkShader(geometryShader, geometryShaderFilename);
			glAttachShader(programID, geometryShader);
		}

		//fragment shader
		const std::string &fragmentShaderFileSource = readEntireFile(shadersParentDirectory + shadersDirectoryName + fragmentShaderFilename);
		const std::string &fragmentShaderSourceStep1 = tokenReplacerShader.replaceTokens(fragmentShaderFileSource, tokens);
		const std::string &fragmentShaderSource = loopUnrollerShader.unrollLoops(fragmentShaderSourceStep1);
		const char *fragmentShaderSourceChar = fragmentShaderSource.c_str();
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSourceChar, nullptr);
		glCompileShader(fragmentShader);
		checkShader(fragmentShader, fragmentShaderFilename);
		glAttachShader(programID, fragmentShader);

		//link
		glLinkProgram(programID);
		checkProgram(programID, vertexShaderFilename + ", " + geometryShaderFilename + ", " + fragmentShaderFilename);

		return programID;
	}

	void ShaderManager::removeProgram(unsigned int programID)
	{
		if(programID!=0)
		{
			clearProgram(programID);
			programs.remove(programID);
		}
	}

	std::string ShaderManager::readEntireFile(const std::string &filename)
	{
		std::string fileContent;
		fileContent.reserve(10000);

		std::ifstream file;
		file.open(filename, std::ios::in|std::ios::binary);
		if(file.fail())
		{
			throw std::invalid_argument("Cannot open the file " + filename + ".");
		}

		char c;
		while (file.get(c))
		{
			fileContent.append(1, c);
		}

		file.close();

		return fileContent;
	}

	void ShaderManager::clearProgram(unsigned int programID)
	{
		if(programID==0)
		{
			return;
		}

		int countAttachedShaders = 0;
		int maxCount = 10;
		auto *attachedShaders = new unsigned int[maxCount];

		glGetAttachedShaders(programID, maxCount, &countAttachedShaders, attachedShaders);

		for(int i=0; i<countAttachedShaders; ++i)
		{
			glDetachShader(programID, attachedShaders[i]);
			glDeleteShader(attachedShaders[i]);
		}

		delete [] attachedShaders;
		glDeleteProgram(programID);
	}

	void ShaderManager::checkShader(unsigned int shaderID, const std::string &shaderFilename)
	{
		int infoLogLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

		if(infoLogLength > 1)
		{
			auto *infoLog = new char[infoLogLength];
			glGetShaderInfoLog(shaderID, infoLogLength, nullptr, infoLog);

			std::ostringstream buffer;
			buffer<<"Error in shader file: "<<shaderFilename<<"."<<std::endl<<infoLog;

			delete [] infoLog;
			throw std::runtime_error(buffer.str());
		}
	}

	void ShaderManager::checkProgram(unsigned int programID, const std::string &shaderFilenames)
	{
		int infoLogLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

		if(infoLogLength > 1)
		{
			auto *infoLog = new char[infoLogLength];
			glGetProgramInfoLog(programID, infoLogLength, nullptr, infoLog);

			std::ostringstream buffer;
			buffer<<"Error in shader file(s): "<<shaderFilenames<<"."<<std::endl<<infoLog;

			delete [] infoLog;
			throw std::runtime_error(buffer.str());
		}
	}

	void ShaderManager::bind(unsigned int programID)
	{
		if(programID!=currentProgramID)
		{
			currentProgramID = programID;
			glUseProgram(currentProgramID);
		}
	}

	unsigned int ShaderManager::getCurrentProgram() const
	{
		return currentProgramID;
	}
}
