#include <algorithm>

#include "VectorPairContainer.h"

namespace urchin
{

	VectorPairContainer::VectorPairContainer()
	{

	}

	VectorPairContainer::~VectorPairContainer()
	{
		for(std::vector<OverlappingPair *>::iterator it = overlappingPairs.begin(); it!=overlappingPairs.end(); ++it)
		{
			delete *it;
		}
	}

	void VectorPairContainer::addOverlappingPair(AbstractWorkBody *body1, AbstractWorkBody *body2)
	{
		uint_fast64_t bodiesId = OverlappingPair::computeBodiesId(body1, body2);

		bool found = false;
		for(std::vector<OverlappingPair *>::iterator it = overlappingPairs.begin(); it!=overlappingPairs.end(); ++it)
		{
			if((*it)->getBodiesId()==bodiesId)
			{
				found = true;
				break;
			}
		}

		if(!found)
		{ //pair doesn't exist: we create it
			overlappingPairs.push_back(new OverlappingPair(body1, body2, bodiesId));
		}
	}

	void VectorPairContainer::removeOverlappingPair(AbstractWorkBody *body1, AbstractWorkBody *body2)
	{
		uint_fast64_t bodiesId = OverlappingPair::computeBodiesId(body1, body2);
		for(std::vector<OverlappingPair *>::iterator it = overlappingPairs.begin(); it!=overlappingPairs.end(); ++it)
		{
			if((*it)->getBodiesId()==bodiesId)
			{
				delete *it;
				overlappingPairs.erase(it);
				break;
			}
		}
	}

	void VectorPairContainer::removeOverlappingPairs(AbstractWorkBody *body)
	{
		std::vector<OverlappingPair *>::iterator it = overlappingPairs.begin();
		while(it!=overlappingPairs.end())
		{
			OverlappingPair *pair = *it;

			if(pair->getBody1()==body || pair->getBody2()==body)
			{
				it = overlappingPairs.erase(it);
				delete pair;
			}else
			{
				++it;
			}
		}
	}

	const std::vector<OverlappingPair *> &VectorPairContainer::getOverlappingPairs() const
	{
		return overlappingPairs;
	}
}
