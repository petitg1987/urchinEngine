#include <GL/gl.h>

#include "utils/display/geometry/aabbox/AABBoxModel.h"

namespace urchin
{

	AABBoxModel::AABBoxModel(const AABBox<float> &aabbox):
			aabbox(aabbox)
	{
		initialize();
	}

	AABBoxModel::~AABBoxModel()
	{

	}

	Matrix4<float> AABBoxModel::retrieveModelMatrix() const
	{
		return Matrix4<float>();
	}

	std::vector<Point3<float>> AABBoxModel::retrieveVertexArray() const
	{
		std::vector<Point3<float>> vertexArray;
		vertexArray.reserve(24);

		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMin().Y, aabbox.getMin().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMin().Y, aabbox.getMin().Z));

		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMax().Y, aabbox.getMin().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMax().Y, aabbox.getMin().Z));

		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMin().Y, aabbox.getMin().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMax().Y, aabbox.getMin().Z));

		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMin().Y, aabbox.getMin().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMax().Y, aabbox.getMin().Z));


		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMin().Y, aabbox.getMin().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMin().Y, aabbox.getMax().Z));

		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMin().Y, aabbox.getMin().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMin().Y, aabbox.getMax().Z));

		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMax().Y, aabbox.getMin().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMax().Y, aabbox.getMax().Z));

		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMax().Y, aabbox.getMin().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMax().Y, aabbox.getMax().Z));


		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMin().Y, aabbox.getMax().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMin().Y, aabbox.getMax().Z));

		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMax().Y, aabbox.getMax().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMax().Y, aabbox.getMax().Z));

		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMin().Y, aabbox.getMax().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMin().X, aabbox.getMax().Y, aabbox.getMax().Z));

		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMin().Y, aabbox.getMax().Z));
		vertexArray.push_back(Point3<float>(aabbox.getMax().X, aabbox.getMax().Y, aabbox.getMax().Z));

		return vertexArray;
	}

	void AABBoxModel::drawGeometry() const
	{
		glDrawArrays(GL_LINES, 0, 24);
	}

}
