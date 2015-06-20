#include <GL/glew.h>
#include <GL/gl.h>
#include <stdexcept>

#include "Skybox.h"
#include "scene/renderer3d/camera/Camera.h"
#include "utils/shader/ShaderManager.h"
#include "resources/MediaManager.h"

namespace urchin
{

	/**
	 * Create black skybox
	 */
	Skybox::Skybox() :
			offsetY(0.0)
	{
		texSkybox = new Image*[6];
		texSkybox[0] = new Image(3, 1, 1, GL_RGB, new unsigned char[3]{0, 0, 0});
		texSkybox[1] = new Image(3, 1, 1, GL_RGB, new unsigned char[3]{0, 0, 0});
		texSkybox[2] = new Image(3, 1, 1, GL_RGB, new unsigned char[3]{0, 0, 0});
		texSkybox[3] = new Image(3, 1, 1, GL_RGB, new unsigned char[3]{0, 0, 0});
		texSkybox[4] = new Image(3, 1, 1, GL_RGB, new unsigned char[3]{0, 0, 0});
		texSkybox[5] = new Image(3, 1, 1, GL_RGB, new unsigned char[3]{0, 0, 0});

		initialize();
	}

	/**
	* @param filenames Filenames of the textures in the following order: X-, X+, Y-, Y+, Z-, Z+
	*/
	Skybox::Skybox(const std::vector<std::string> &filenames) :
			offsetY(0.0)
	{
		if(filenames.size()!=6)
		{
			throw std::invalid_argument("There is no 6 skybox filenames.");
		}

		//create the textures
		texSkybox = new Image*[6];
		texSkybox[0] = MediaManager::instance()->getMedia<Image>(filenames[0]);
		texSkybox[1] = MediaManager::instance()->getMedia<Image>(filenames[1]);
		texSkybox[2] = MediaManager::instance()->getMedia<Image>(filenames[2]);
		texSkybox[3] = MediaManager::instance()->getMedia<Image>(filenames[3]);
		texSkybox[4] = MediaManager::instance()->getMedia<Image>(filenames[4]);
		texSkybox[5] = MediaManager::instance()->getMedia<Image>(filenames[5]);

		initialize();
	}

	Skybox::~Skybox()
	{
		glDeleteTextures(1, &textureID);
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(2, bufferIDs);

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
		const float SIZE = 10.0;
		
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		for (int i=0; i<6; i++)
		{
			glTexImage2D(cubeMapTarget[i], 0, texSkybox[i]->getInternalFormat(), texSkybox[i]->getWidth(), texSkybox[i]->getHeight(), 0, texSkybox[i]->getFormat(), GL_UNSIGNED_BYTE, texSkybox[i]->getTexels());
			delete texSkybox[i];
		}
		delete [] texSkybox;

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//create vertex array
		const float vertexArray[] = {
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

		//create texture coordinates array
		const float stArray[] = {
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
		
		//visual
		skyboxShader = ShaderManager::instance()->createProgram("skybox.vert", "skybox.frag");

		ShaderManager::instance()->bind(skyboxShader);
		glGenBuffers(2, bufferIDs);
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), &vertexArray[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
		glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, false, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(stArray), &stArray[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_TEX_COORD);
		glVertexAttribPointer(SHADER_TEX_COORD, 3, GL_FLOAT, false, 0, 0);

		mProjectionLoc = glGetUniformLocation(skyboxShader, "mProjection");
		mViewLoc = glGetUniformLocation(skyboxShader, "mView");
		diffuseTexSamplerLoc = glGetUniformLocation(skyboxShader, "diffuseTexture");
	}

	void Skybox::onCameraProjectionUpdate(const Camera *const camera)
	{
		ShaderManager::instance()->bind(skyboxShader);
		glUniformMatrix4fv(mProjectionLoc, 1, false, (const float*)camera->getProjectionMatrix());
	}

	void Skybox::setOffsetY(float offsetY)
	{
		this->offsetY = offsetY;
	}

	void Skybox::display(const Matrix4<float> &viewMatrix, const Point3<float> &cameraPosition)
	{
		ShaderManager::instance()->bind(skyboxShader);
		
		translationMatrix.buildTranslation(cameraPosition.X, cameraPosition.Y + offsetY, cameraPosition.Z);
		glUniformMatrix4fv(mViewLoc, 1, false, (const float*)(viewMatrix * translationMatrix));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(diffuseTexSamplerLoc, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_QUADS, 0, 24);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}

}
