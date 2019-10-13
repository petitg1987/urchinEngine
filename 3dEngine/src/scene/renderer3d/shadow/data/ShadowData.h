#ifndef URCHINENGINE_SHADOWDATA_H
#define URCHINENGINE_SHADOWDATA_H

#include <vector>
#include <set>
#include <memory>
#include "UrchinCommon.h"

#include "FrustumShadowData.h"
#include "scene/renderer3d/light/Light.h"
#include "utils/filter/TextureFilter.h"

namespace urchin
{

	/**
	* Shadow execution data for a light
	*/
	class ShadowData
	{
		public:
			ShadowData(const Light *, unsigned int);
			~ShadowData();

			void setFboID(unsigned int);
			unsigned int getFboID() const;

			void setDepthTextureID(unsigned int);
			unsigned int getDepthTextureID() const;
			void setShadowMapTextureID(unsigned int);
			unsigned int getShadowMapTextureID() const;

			void addTextureFilter(const std::shared_ptr<const TextureFilter> &);
			void applyTextureFilters();
			unsigned int getFilteredShadowMapTextureID() const;

			void setLightViewMatrix(const Matrix4<float> &);
			const Matrix4<float> &getLightViewMatrix() const;

			unsigned int getNbFrustumShadowData() const;
			FrustumShadowData *getFrustumShadowData(std::size_t);
			const FrustumShadowData *getFrustumShadowData(std::size_t) const;

			const std::vector<Model *> &retrieveModels() const;

		private:
			const Light *const light;

			unsigned int fboID; //frame buffer object ID containing shadow map(s)
			unsigned int depthTextureID; //depth texture ID
			unsigned int shadowMapTextureID; //shadow map texture ID (variance shadow map)

			std::vector<std::shared_ptr<const TextureFilter>> textureFilters; //shadow map filters

			Matrix4<float> lightViewMatrix;
			std::vector<FrustumShadowData *> frustumShadowData;
			mutable std::vector<Model *> models;
	};

}

#endif
