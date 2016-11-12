#ifndef ENGINE_ISLANDELEMENT_H
#define ENGINE_ISLANDELEMENT_H

namespace urchin
{

	/**
	* An element which can belong to an island.
	*/
	class IslandElement
	{
		public:
			IslandElement();
			virtual ~IslandElement();

			virtual void setIslandElementId(unsigned int) = 0;
			virtual unsigned int getIslandElementId() const = 0;

			virtual bool isActive() const = 0;
	};

}

#endif
