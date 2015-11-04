#ifndef ENGINE_SHADOWDATA_H
#define ENGINE_SHADOWDATA_H

#include <vector>
#include <set>
#include <memory>
#include "UrchinCommon.h"

#include "FrustumShadowData.h"
#include "scene/renderer3d/light/Light.h"
#include "scene/renderer3d/filter/TextureFilter.h"

namespace urchin
{

	/**
	* Shadow execution data for a light
	*/
	class ShadowData
	{
		public:
			ShadowData(const Light *const, unsigned int);
			~ShadowData();

			void setFboID(unsigned int);
			unsigned int getFboID() const;

			void setDepthTextureID(unsigned int);
			unsigned int getDepthTextureID() const;
			void setShadowMapTextureID(unsigned int);
			unsigned int getShadowMapTextureID() const;

			void setDownSample2xFilter(std::shared_ptr<const TextureFilter>);
			std::shared_ptr<const TextureFilter> getDownSample2xFilter() const;

			void setDownSample4xFilter(std::shared_ptr<const TextureFilter>);
			std::shared_ptr<const TextureFilter> getDownSample4xFilter() const;

			void setBlurFilter(std::shared_ptr<const TextureFilter>);
			std::shared_ptr<const TextureFilter> getBlurFilter() const;

			void setLightViewMatrix(const Matrix4<float> &);
			const Matrix4<float> &getLightViewMatrix() const;

			unsigned int getNbFrustumShadowData() const;
			FrustumShadowData *getFrustumShadowData(unsigned int);
			const FrustumShadowData *getFrustumShadowData(unsigned int) const;

			std::set<Model *> retrieveModels() const;

		private:
			const Light *const light;

			unsigned int fboID; //frame buffer object ID containing shadow map(s)
			unsigned int depthTextureID; //depth texture ID
			unsigned int shadowMapTextureID; //shadow map texture ID (variance shadow map)

			//shadow map filters
			std::shared_ptr<const TextureFilter> downSample2xFilter;
			std::shared_ptr<const TextureFilter> downSample4xFilter;
			std::shared_ptr<const TextureFilter> blurFilter;

			Matrix4<float> lightViewMatrix;
			std::vector<FrustumShadowData *> frustumShadowData;
	};

}

#endif
