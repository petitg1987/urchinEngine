#ifndef URCHINENGINE_BODYENDPOINT_H
#define URCHINENGINE_BODYENDPOINT_H

#include <iostream>

namespace urchin
{

	class BodyBox;

	/**
	* Body end point of AABB used during broad phase
	*/
	class BodyEndPoint
	{
		public:
			BodyEndPoint();

			void setBodyBox(BodyBox *);
			BodyBox *getBodyBox() const;

			void setIsMin(bool);
			bool isMin() const;

			void setValue(float);
			float getValue() const;

		private:
			BodyBox *bodyBox;
			bool bIsMin;
			float value; //minimum or maximum value
	};

}

#endif
