#include <GL/glew.h>
#include <GL/gl.h>

#include "ShadowUniform.h"

namespace urchin
{

	ShadowUniform::ShadowUniform() :
			CustomUniform(),
			mModelProjectionMatrixLoc(0),
			shadowData(nullptr)
	{

	}

	ShadowUniform::~ShadowUniform()
	{

	}

	void ShadowUniform::setProjectionMatricesLocation(int mModelProjectionMatrixLoc)
	{
		this->mModelProjectionMatrixLoc = mModelProjectionMatrixLoc;
	}

	void ShadowUniform::setUniformData(const ShadowData *shadowData)
	{
		this->shadowData = shadowData;
	}

	void ShadowUniform::loadCustomUniforms()
	{
		std::vector<Matrix4<float>> projectionMatrices;
		projectionMatrices.reserve(shadowData->getNbFrustumShadowData());

		for(unsigned int i=0; i<shadowData->getNbFrustumShadowData(); ++i)
		{
			projectionMatrices.push_back(shadowData->getFrustumShadowData(i)->getLightProjectionMatrix());
		}

		glUniformMatrix4fv(mModelProjectionMatrixLoc, projectionMatrices.size(), false, (const float*)projectionMatrices[0]);
	}

}
