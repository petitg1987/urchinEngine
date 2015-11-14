#include <GL/gl.h>
#include <stdexcept>
#include <map>
#include <string>

#include "AntiAliasingApplier.h"
#include "utils/shader/ShaderManager.h"
#include "utils/shader/TokenReplacerShader.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

#define DEFAULT_AA_QUALITY AntiAliasingApplier::Quality::VERY_HIGH

namespace urchin
{

	AntiAliasingApplier::AntiAliasingApplier() :
		isInitialized(false),
		quality(DEFAULT_AA_QUALITY),
		sceneWidth(-1),
		sceneHeight(-1),
		fxaaShader(0),
		texLoc(0),
		invSceneSizeLoc(0)
	{
	
	}
	
	AntiAliasingApplier::~AntiAliasingApplier()
	{
		ShaderManager::instance()->removeProgram(fxaaShader);
	}

	void AntiAliasingApplier::initialize()
	{
		if(isInitialized)
		{
			throw std::runtime_error("Anti aliasing applier is already initialized.");
		}

		loadFxaaShader();

		quadDisplayer = std::make_shared<QuadDisplayerBuilder>()
				->build();

		isInitialized = true;
	}

	void AntiAliasingApplier::loadFxaaShader()
	{
		std::map<std::string, std::string> fxaaTokens;
		fxaaTokens["QUALITY"] = std::to_string(static_cast<int>(quality));

		ShaderManager::instance()->removeProgram(fxaaShader);
		fxaaShader = ShaderManager::instance()->createProgram("fxaa.vert", "fxaa.frag", fxaaTokens);

		ShaderManager::instance()->bind(fxaaShader);
		texLoc = glGetUniformLocation(fxaaShader, "tex");
		invSceneSizeLoc = glGetUniformLocation(fxaaShader, "invSceneSize");
		glUniform2f(invSceneSizeLoc, 1.0f/(float)sceneWidth, 1.0f/(float)sceneHeight);
	}

	void AntiAliasingApplier::onResize(int width, int height)
	{
		ShaderManager::instance()->bind(fxaaShader);

		this->sceneWidth = width;
		this->sceneHeight = height;

		glUniform2f(invSceneSizeLoc, 1.0f/(float)sceneWidth, 1.0f/(float)sceneHeight);
	}

	void AntiAliasingApplier::setQuality(Quality quality)
	{
		this->quality = quality;

		loadFxaaShader();
	}

	void AntiAliasingApplier::applyOn(unsigned int textureId)
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Anti aliasing applier must be initialized before applied.");
		}

		ShaderManager::instance()->bind(fxaaShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glUniform1i(texLoc, 0);

		quadDisplayer->display();
	}

}
