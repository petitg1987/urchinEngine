#ifndef URCHINENGINE_MESHPARAMETER_H
#define URCHINENGINE_MESHPARAMETER_H

#include "UrchinCommon.h"

namespace urchin
{

	class MeshParameter
	{
		public:
			MeshParameter();
			~MeshParameter();

			void setDiffuseTextureUnit(int);
			int getDiffuseTextureUnit() const;

			void setNormalTextureUnit(int);
			int getNormalTextureUnit() const;

			void setAmbientFactorLoc(int);
			int getAmbientFactorLoc() const;

		private:
			int diffuseTextureUnit;
			int normalTextureUnit;
			int ambientFactorLoc;
	};

}

#endif
