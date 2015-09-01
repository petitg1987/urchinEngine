#include <fstream>
#include <stdexcept>
#include <GL/glew.h>
#include <GL/gl.h>

#include "utils/shader/ShaderManager.h"

namespace urchin
{

	ShaderManager::ShaderManager() : Singleton<ShaderManager>(),
			shadersLocation(ConfigService::instance()->getStringValue("shaders.shadersLocation")),
			currentProgramID(0)
	{
		this->shadersWorkingDirectory = FileSystem::instance()->getWorkingDirectory();
	}

	ShaderManager::~ShaderManager()
	{
		for(std::list<unsigned int>::const_iterator it = programs.begin(); it!=programs.end(); ++it)
		{
			clearProgram(*it);
		}
	}

	void ShaderManager::replaceShadersWorkingDirectoryBy(const std::string &shadersWorkingDirectory)
	{
		this->shadersWorkingDirectory = shadersWorkingDirectory;
	}

	unsigned int ShaderManager::createProgram(const std::string &vertexShaderFilename, const std::string &fragmentShaderFilename)
	{
		std::map<TokenReplacerShader::ShaderToken, std::string> emptyTokens;
		return createProgram(vertexShaderFilename, fragmentShaderFilename, emptyTokens);
	}

	unsigned int ShaderManager::createProgram(const std::string &vertexShaderFilename, const std::string &fragmentShaderFilename,
			const std::map<TokenReplacerShader::ShaderToken, std::string> &tokens)
	{
		unsigned int programID = glCreateProgram();
		programs.push_back(programID);

		//vertex shader
		const std::string &vertexShaderFileSource = readEntireFile(shadersWorkingDirectory + shadersLocation + vertexShaderFilename);
		const std::string &vertexShaderSourceStep1 = tokenReplacerShader.replaceTokens(vertexShaderFileSource, tokens);
		const std::string &vertexShaderSource = loopUnrollerShader.unrollLoops(vertexShaderSourceStep1);
		const char *vertexShaderSourceChar = vertexShaderSource.c_str();
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShader, 1, &vertexShaderSourceChar, nullptr);
		glCompileShader(vertexShader);
		checkShader(vertexShader, vertexShaderFilename);
		glAttachShader(programID, vertexShader);

		//fragment shader
		const std::string &fragmentShaderFileSource = readEntireFile(shadersWorkingDirectory + shadersLocation + fragmentShaderFilename);
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
		checkProgram(programID, vertexShaderFilename + ", " + fragmentShaderFilename);

		return programID;
	}

	void ShaderManager::setGeometryShader(unsigned int programID, const std::string &geometryShaderFilename)
	{
		std::map<TokenReplacerShader::ShaderToken, std::string> emptyTokens;
		setGeometryShader(programID, geometryShaderFilename, emptyTokens);
	}

	void ShaderManager::setGeometryShader(unsigned int programID, const std::string &geometryShaderFilename,
			const std::map<TokenReplacerShader::ShaderToken, std::string> &tokens)
	{
		//geometry shader
		const std::string &geometryShaderFileSource = readEntireFile(shadersWorkingDirectory + shadersLocation + geometryShaderFilename);
		const std::string &geometryShaderSourceStep1 = tokenReplacerShader.replaceTokens(geometryShaderFileSource, tokens);
		const std::string &geometryShaderSource = loopUnrollerShader.unrollLoops(geometryShaderSourceStep1);
		const char *geometryShaderSourceChar = geometryShaderSource.c_str();
		unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

		glShaderSource(geometryShader, 1, &geometryShaderSourceChar, nullptr);
		glCompileShader(geometryShader);
		checkShader(geometryShader, geometryShaderFilename);
		glAttachShader(programID, geometryShader);

		//link
		glLinkProgram(programID);
		checkProgram(programID, geometryShaderFilename);
	}

	void ShaderManager::removeProgram(unsigned int programID)
	{
		clearProgram(programID);
		programs.remove(programID);
	}

	std::string ShaderManager::readEntireFile(const std::string &filename)
	{
		std::string fileContent;
		fileContent.reserve(10000);

		std::ifstream file;
		file.open(filename.c_str(), std::ios::in|std::ios::binary);
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
		unsigned int *attachedShaders = new unsigned int[maxCount];

		glGetAttachedShaders(programID, maxCount, &countAttachedShaders, attachedShaders);

		for(int i=0; i<countAttachedShaders; ++i)
		{
			glDetachShader(programID, attachedShaders[i]);
			glDeleteShader(attachedShaders[i]);
		}

		delete []attachedShaders;
		glDeleteProgram(programID);
	}

	void ShaderManager::checkShader(unsigned int shaderID, const std::string &shaderFilename)
	{
		int infoLogLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

		if(infoLogLength > 1)
		{
			char *infoLog = new char[infoLogLength];
			glGetShaderInfoLog(shaderID, infoLogLength, nullptr, infoLog);

			std::ostringstream buffer;
			buffer<<"Error in shader file: "<<shaderFilename<<"."<<std::endl<<infoLog;

			delete [] infoLog;
			throw std::runtime_error(buffer.str());
		}
	}

	void ShaderManager::checkProgram(unsigned int programID, const std::string &shaderFilename)
	{
		int infoLogLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

		if(infoLogLength > 1)
		{
			char *infoLog = new char[infoLogLength];
			glGetProgramInfoLog(programID, infoLogLength, nullptr, infoLog);

			std::ostringstream buffer;
			buffer<<"Error in shader file(s): "<<shaderFilename<<"."<<std::endl<<infoLog;

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
