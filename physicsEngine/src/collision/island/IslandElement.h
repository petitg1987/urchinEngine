#ifndef URCHINENGINE_ISLANDELEMENT_H
#define URCHINENGINE_ISLANDELEMENT_H

namespace urchin
{

	/**
	* An element which can belong to an island.
	*/
	class IslandElement
	{
		public:
			virtual ~IslandElement() = default;

			virtual void setIslandElementId(unsigned int) = 0;
			virtual unsigned int getIslandElementId() const = 0;

			virtual bool isActive() const = 0;
	};

}

#endif
