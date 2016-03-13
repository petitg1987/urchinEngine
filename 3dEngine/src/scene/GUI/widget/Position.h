#ifndef ENGINE_WIDGETPOSITION_H
#define ENGINE_WIDGETPOSITION_H

namespace urchin
{

	class Position
	{
		public:
			enum PositionType{
				PIXEL,
				PERCENTAGE
			};

			Position(float, PositionType, float, PositionType);
			Position(float, float, PositionType);

			float getPositionX() const;
			PositionType getPositionTypeX() const;

			float getPositionY() const;
			PositionType getPositionTypeY() const;

		private:
			float positionX, positionY;
			PositionType positionTypeX, positionTypeY;
	};

}

#endif
