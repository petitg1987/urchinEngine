#include <GL/glew.h>

#include "utils/display/geometry/GeometryModel.h"
#include "utils/shader/ShaderManager.h"

namespace urchin
{

	GeometryModel::GeometryModel() :
			color(Vector4<float>(0.0, 1.0, 0.0, 1.0)),
			polygonMode(WIREFRAME),
			lineSize(1.3),
			blendMode(NONE),
            alwaysVisible(false)
	{
		shader = ShaderManager::instance()->createProgram("displayGeometry.vert", "", "displayGeometry.frag");

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

	Vector4<float> GeometryModel::getColor() const
	{
		return color;
	}

	void GeometryModel::setColor(float red, float green, float blue, float alpha)
	{
		this->color = Vector4<float>(red, green, blue, alpha);
	}

	GeometryModel::PolygonMode GeometryModel::getPolygonMode() const
	{
		return polygonMode;
	}

	void GeometryModel::setPolygonMode(PolygonMode polygonMode)
	{
		this->polygonMode = polygonMode;
	}

	void GeometryModel::setLineSize(float lineSize)
	{
		this->lineSize = lineSize;
	}

	GeometryModel::BlendMode GeometryModel::getBlendMode() const
	{
		return blendMode;
	}

	void GeometryModel::setBlendMode(BlendMode blendMode)
	{
		this->blendMode = blendMode;
	}

    bool GeometryModel::isAlwaysVisible() const
    {
        return alwaysVisible;
    }

    void GeometryModel::setAlwaysVisible(bool alwaysVisible)
    {
        this->alwaysVisible = alwaysVisible;
    }

	void GeometryModel::initialize()
	{
		modelMatrix = retrieveModelMatrix();
		std::vector<Point3<float>> vertexArray = retrieveVertexArray();

		glBindVertexArray(vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
		glBufferData(GL_ARRAY_BUFFER, vertexArray.size()*sizeof(Point3<float>), &vertexArray[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
		glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	void GeometryModel::display(const Matrix4<float> &viewMatrix) const
    {
        unsigned int shaderSaved = ShaderManager::instance()->getCurrentProgram();
        ShaderManager::instance()->bind(shader);

        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float *) projectionMatrix);
        glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float *) (viewMatrix * modelMatrix));
        glUniform4fv(colorLoc, 1, (const float *) color);

        glBindVertexArray(vertexArrayObject);

		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode==WIREFRAME ? GL_LINE : GL_FILL);
		glLineWidth(lineSize);
		if(blendMode==ONE_MINUS_SRC_ALPHA)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
        if (isAlwaysVisible())
        {
            glDisable(GL_DEPTH_TEST);
        }

		drawGeometry();

        if (isAlwaysVisible())
        {
            glEnable(GL_DEPTH_TEST);
        }
		if(blendMode!=NONE)
		{
			glDisable(GL_BLEND);
		}
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		ShaderManager::instance()->bind(shaderSaved);
	}

}
