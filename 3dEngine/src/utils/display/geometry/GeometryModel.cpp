#include <GL/glew.h>
#include <GL/gl.h>

#include "utils/display/geometry/GeometryModel.h"
#include "utils/shader/ShaderManager.h"

namespace urchin
{

	GeometryModel::GeometryModel() :
			red(0.0),
			green(1.0),
			blue(0.0)
	{
		shader = ShaderManager::instance()->createProgram("displayGeometry.vert", "displayGeometry.frag");

		mProjectionLoc = glGetUniformLocation(shader, "mProjection");
		mViewLoc = glGetUniformLocation(shader, "mView");
		colorLoc = glGetUniformLocation(shader, "color");

		glGenBuffers(1, bufferIDs);
		glGenVertexArrays(1, &vertexArrayObject);
	}

	GeometryModel::~GeometryModel()
	{
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(1, bufferIDs);

		ShaderManager::instance()->removeProgram(shader);
	}

	void GeometryModel::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix)
	{
		this->projectionMatrix = projectionMatrix;
	}

	Vector3<float> GeometryModel::getColor() const
	{
		return Vector3<float>(red, green, blue);
	}

	void GeometryModel::setColor(float red, float green, float blue)
	{
		this->red = red;
		this->green = green;
		this->blue = blue;
	}

	void GeometryModel::initialize()
	{
		modelMatrix = retrieveModelMatrix();
		vertexArray = retrieveVertexArray();
	}

	void GeometryModel::display(const Matrix4<float> &viewMatrix) const
	{
		unsigned int shaderSaved = ShaderManager::instance()->getCurrentProgram();
		ShaderManager::instance()->bind(shader);

		glUniformMatrix4fv(mProjectionLoc, 1, false, (const float*)projectionMatrix);
		glUniformMatrix4fv(mViewLoc, 1, false, (const float*)(viewMatrix * modelMatrix));
		glUniform3fv(colorLoc, 1, (const float*)Vector3<float>(red, green, blue));

		glBindVertexArray(vertexArrayObject);

		GLboolean cullFaceActive;
		glGetBooleanv(GL_CULL_FACE, &cullFaceActive);
		if(cullFaceActive)
		{
			glDisable(GL_CULL_FACE);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(1.3);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
		glBufferData(GL_ARRAY_BUFFER, vertexArray.size()*sizeof(Point3<float>), &vertexArray[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
		glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, false, 0, 0);
		drawGeometry();

		if(cullFaceActive)
		{
			glEnable(GL_CULL_FACE);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		ShaderManager::instance()->bind(shaderSaved);
	}

}
