#include <GL/glew.h>
#include <stdexcept>

#include "Skybox.h"
#include "utils/shader/ShaderManager.h"
#include "resources/MediaManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

#define SKYBOX_DEFAULT_SIZE 1

namespace urchin
{

	/**
	 * Create black skybox
	 */
	Skybox::Skybox() :
            textureID(0),
			offsetY(0.0f),
            skyboxShader(0),
            mProjectionLoc(0),
            mViewLoc(0)
	{
		texSkybox = new Image*[6];
        for(unsigned int i=0; i < 6; ++i)
        {
            texSkybox[i] = new Image(SKYBOX_DEFAULT_SIZE, SKYBOX_DEFAULT_SIZE, Image::IMAGE_RGB, std::vector<unsigned char>(SKYBOX_DEFAULT_SIZE * SKYBOX_DEFAULT_SIZE * 3, 0));
        }

		initialize();
	}

	/**
	* @param filenames Filenames of the textures in the following order: X-, X+, Y-, Y+, Z-, Z+
	*/
	Skybox::Skybox(const std::vector<std::string> &filenames) :
            filenames(filenames),
            textureID(0),
            offsetY(0.0),
            skyboxShader(0),
            mProjectionLoc(0),
            mViewLoc(0)
	{
		if(filenames.size() != 6)
		{
			throw std::invalid_argument("There is no 6 skybox filenames.");
		}

		//create the textures
		texSkybox = new Image*[6];
		unsigned int skyboxSize = SKYBOX_DEFAULT_SIZE;
		for(std::size_t i=0; i < 6; ++i)
        {
		    if(!filenames[i].empty())
            {
                texSkybox[i] = MediaManager::instance()->getMedia<Image>(filenames[i]);
                skyboxSize = texSkybox[i]->getWidth();
            }
        }

        for(std::size_t i=0; i < 6; ++i)
        {
            if(filenames[i].empty())
            {
                texSkybox[i] = new Image(skyboxSize, skyboxSize, Image::IMAGE_RGB, std::vector<unsigned char>(skyboxSize * skyboxSize * 3, 0));
            }
        }

        for(std::size_t i=0; i < 6 - 1; ++i)
        {
            unsigned int widthSize = texSkybox[i]->getWidth();
            unsigned int heightSize = texSkybox[i]->getHeight();
            unsigned int nextWidthSize = texSkybox[i + 1]->getWidth();

            if(texSkybox[i]->getWidth() != texSkybox[i]->getHeight())
            {
                clearTexSkybox();
                throw std::runtime_error("Skybox image must be a square. Present image size: " + std::to_string(widthSize) + "x" + std::to_string(heightSize));
            }else if(widthSize != nextWidthSize)
            {
                clearTexSkybox();
                throw std::runtime_error("All skybox images must have the same size: " + std::to_string(widthSize) + " != " + std::to_string(nextWidthSize));
            }
        }

		initialize();
	}

	Skybox::~Skybox()
	{
        clearTexSkybox();
		glDeleteTextures(1, &textureID);
		ShaderManager::instance()->removeProgram(skyboxShader);
	}

	void Skybox::initialize()
	{
		GLenum cubeMapTarget[6] = {           
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z
		};
		const float SIZE = 10.0f;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		for (std::size_t i=0; i<6; i++)
		{
			glTexImage2D(cubeMapTarget[i], 0, texSkybox[i]->retrieveInternalFormat(), texSkybox[i]->getWidth(), texSkybox[i]->getHeight(), 0, texSkybox[i]->retrieveFormat(), GL_UNSIGNED_BYTE, &texSkybox[i]->getTexels()[0]);
		}
        clearTexSkybox();

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//visual
		skyboxShader = ShaderManager::instance()->createProgram("skybox.vert", "", "skybox.frag");

		ShaderManager::instance()->bind(skyboxShader);
		mProjectionLoc = glGetUniformLocation(skyboxShader, "mProjection");
		mViewLoc = glGetUniformLocation(skyboxShader, "mView");
		int diffuseTexSamplerLoc = glGetUniformLocation(skyboxShader, "diffuseTexture");
		glUniform1i(diffuseTexSamplerLoc, GL_TEXTURE0-GL_TEXTURE0);

		std::vector<float> vertexCoord = {
			//x negative:
			-SIZE, -SIZE, SIZE, -SIZE, SIZE, SIZE, -SIZE, SIZE, -SIZE, -SIZE, -SIZE, -SIZE,
			//x positive:
			SIZE, -SIZE, -SIZE, SIZE, SIZE, -SIZE, SIZE, SIZE, SIZE, SIZE, -SIZE, SIZE,
			//y negative:
			-SIZE, -SIZE, -SIZE, SIZE, -SIZE, -SIZE, SIZE, -SIZE, SIZE, -SIZE, -SIZE, SIZE,
			//y positive:
			-SIZE, SIZE, SIZE,SIZE, SIZE, SIZE, SIZE, SIZE, -SIZE, -SIZE, SIZE, -SIZE,
			//z negative:
			-SIZE, -SIZE, -SIZE, -SIZE, SIZE, -SIZE, SIZE, SIZE, -SIZE, SIZE, -SIZE, -SIZE,
			//z positive:
			SIZE, -SIZE, SIZE, SIZE, SIZE, SIZE, -SIZE, SIZE, SIZE, -SIZE, -SIZE, SIZE,
		};

		std::vector<float> textureCoord = {
			//x negative:
			-SIZE, SIZE, SIZE, -SIZE, -SIZE, SIZE, -SIZE, -SIZE, -SIZE, -SIZE, SIZE, -SIZE,
			//x positive:
			SIZE, SIZE, -SIZE, SIZE, -SIZE, -SIZE, SIZE, -SIZE, SIZE, SIZE, SIZE, SIZE,
			//y negative:
			-SIZE, -SIZE, SIZE, SIZE, -SIZE, SIZE, SIZE, -SIZE, -SIZE, -SIZE, -SIZE, -SIZE,
			//y positive:
			-SIZE, SIZE, -SIZE, SIZE, SIZE, -SIZE, SIZE, SIZE, SIZE, -SIZE, SIZE, SIZE,
			//z negative:
			-SIZE, SIZE, -SIZE, -SIZE, -SIZE, -SIZE, SIZE, -SIZE, -SIZE, SIZE, SIZE, -SIZE,
			//z positive:
			SIZE, SIZE, SIZE, SIZE, -SIZE, SIZE, -SIZE, -SIZE, SIZE, -SIZE, SIZE, SIZE,
		};
		
		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->numberOfQuad(6)
				->dimension(3) //3D
				->vertexData(GL_FLOAT, &vertexCoord[0], false)
				->textureData(GL_FLOAT, &textureCoord[0], false)
				->build();
	}

    void Skybox::clearTexSkybox()
    {
	    if(texSkybox != nullptr)
        {
            for (std::size_t i = 0; i < 6; i++)
            {
                texSkybox[i]->release();
            }
            delete[] texSkybox;
        }
        texSkybox = nullptr;
    }

	void Skybox::onCameraProjectionUpdate(const Camera *const camera)
	{
		ShaderManager::instance()->bind(skyboxShader);
		glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)camera->getProjectionMatrix());
	}

    float Skybox::getOffsetY() const
    {
	    return offsetY;
    }

	void Skybox::setOffsetY(float offsetY)
	{
		this->offsetY = offsetY;
	}

    const std::vector<std::string> &Skybox::getFilenames() const
    {
	    return filenames;
    }

	void Skybox::display(const Matrix4<float> &viewMatrix, const Point3<float> &cameraPosition)
	{
		ShaderManager::instance()->bind(skyboxShader);
		
		translationMatrix.buildTranslation(cameraPosition.X, cameraPosition.Y + offsetY, cameraPosition.Z);
		glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*)(viewMatrix * translationMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		
		quadDisplayer->display();
	}

}
