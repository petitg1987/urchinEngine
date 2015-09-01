#include <iostream>
#include <limits>

#include "collision/broadphase/SweepAndPrune.h"
#include "collision/broadphase/VectorPairContainer.h"

#define MAX_END_POINTS 16383

namespace urchin
{

	SweepAndPrune::SweepAndPrune() :
		defaultPairContainer(new VectorPairContainer()),
		contactBreakingThreshold(ConfigService::instance()->getFloatValue("narrowPhase.contactBreakingThreshold"))
	{
		for(unsigned int axis=0; axis<3; ++axis)
		{
			sortedEndPoints[axis] = new BodyEndPoint[MAX_END_POINTS];
		}
		createSentinels();
	}

	SweepAndPrune::~SweepAndPrune()
	{
		for(unsigned int axis=0; axis<3; ++axis)
		{
			delete [] sortedEndPoints[axis];
		}

		for(std::map<AbstractWorkBody *, BodyBox *>::iterator it = bodiesBox.begin(); it!=bodiesBox.end(); ++it)
		{
			delete it->second;
		}

		delete defaultPairContainer;
	}

	/**
	 * @param alternativePairContainer Alternative pair container to default one
	 */
	void SweepAndPrune::addBody(AbstractWorkBody *body, PairContainer *alternativePairContainer)
	{
		BodyBox *bodyBox = new BodyBox(body, alternativePairContainer);
		const AABBox<float> &aabbox = body->getShape()->toAABBox(body->getPhysicsTransform());

		unsigned int limit = bodiesBox.size() * 2 + 2; //bodies size * both points (min & max) + 2 sentinels

		for(unsigned int axis=0; axis<3; ++axis)
		{
			//move max sentinel
			sortedEndPoints[axis][limit+1] = sortedEndPoints[axis][limit-1];

			//insert minimum
			sortedEndPoints[axis][limit-1].setBodyBox(bodyBox);
			sortedEndPoints[axis][limit-1].setIsMin(true);
			sortedEndPoints[axis][limit-1].setValue(aabbox.getMin()[axis] - contactBreakingThreshold);
			bodyBox->min[axis] = limit-1;

			//insert maximum
			sortedEndPoints[axis][limit].setBodyBox(bodyBox);
			sortedEndPoints[axis][limit].setIsMin(false);
			sortedEndPoints[axis][limit].setValue(aabbox.getMax()[axis] + contactBreakingThreshold);
			bodyBox->max[axis] = limit;
		}

		sortMinDown(0, bodyBox->min[0], false);
		sortMaxDown(0, bodyBox->max[0], false);
		sortMinDown(1, bodyBox->min[1], false);
		sortMaxDown(1, bodyBox->max[1], false);
		sortMinDown(2, bodyBox->min[2], true);
		sortMaxDown(2, bodyBox->max[2], true);

		//add body to bodies box map
		bodiesBox[body] = bodyBox;

		#ifdef _DEBUG
			//printSortedEndPoints();
		#endif
	}

	void SweepAndPrune::removeBody(AbstractWorkBody *body)
	{
		BodyBox *bodyBox = bodiesBox[body];

		//move minimum and maximum end points to end of array (before maximum sentinel)
		for(unsigned int axis=0; axis<3; ++axis)
		{
			unsigned int minEndPointIndex = bodyBox->min[axis];
			unsigned int maxEndPointIndex = bodyBox->max[axis];

			sortedEndPoints[axis][maxEndPointIndex].setValue(getMaxSentinelValue() - std::numeric_limits<float>::epsilon());
			sortMaxUp(axis, maxEndPointIndex, false);

			sortedEndPoints[axis][minEndPointIndex].setValue(getMaxSentinelValue() - std::numeric_limits<float>::epsilon());
			sortMinUp(axis, minEndPointIndex, false);
		}

		//erase first end points with maximum sentinel
		unsigned int limit = bodiesBox.size() * 2 + 2; //bodies size * both points (min & max) + 2 sentinels
		for(unsigned int axis=0; axis<3; ++axis)
		{
			sortedEndPoints[axis][limit-3] = sortedEndPoints[axis][limit-1];
		}

		//remove overlapping pairs
		defaultPairContainer->removeOverlappingPairs(body);
		for(std::map<AbstractWorkBody *, BodyBox *>::iterator it = bodiesBox.begin(); it!=bodiesBox.end(); ++it)
		{
			if(it->second->hasAlternativePairContainer())
			{
				it->second->getAlternativePairContainer()->removeOverlappingPairs(body);
			}
		}

		//remove body from bodies box map
		bodiesBox.erase(body);
		delete bodyBox;

		#ifdef _DEBUG
			//printSortedEndPoints();
		#endif
	}

	void SweepAndPrune::updateBodies()
	{
		for(std::map<AbstractWorkBody *, BodyBox *>::iterator it = bodiesBox.begin(); it!=bodiesBox.end(); ++it)
		{
			AbstractWorkBody *body = it->first;
			if(body->isActive())
			{
				BodyBox *bodyBox = it->second;
				const AABBox<float> &aabbox = body->getShape()->toAABBox(body->getPhysicsTransform());

				for(unsigned int axis=0; axis<3; ++axis)
				{
					unsigned int minEndPointIndex = bodyBox->min[axis];
					unsigned int maxEndPointIndex = bodyBox->max[axis];

					const float aabboxMin = aabbox.getMin()[axis] - contactBreakingThreshold;
					const float aabboxMax = aabbox.getMax()[axis] + contactBreakingThreshold;
					const float minDelta = aabboxMin - sortedEndPoints[axis][minEndPointIndex].getValue();
					const float maxDelta = aabboxMax - sortedEndPoints[axis][maxEndPointIndex].getValue();

					sortedEndPoints[axis][bodyBox->min[axis]].setValue(aabboxMin);
					sortedEndPoints[axis][bodyBox->max[axis]].setValue(aabboxMax);

					//1. add overlap if necessary
					if(minDelta < 0.0f)
					{ //new minimum point is smaller
						sortMinDown(axis, minEndPointIndex, true);
					}
					if(maxDelta > 0.0f)
					{ //new maximum point is bigger
						sortMaxUp(axis, maxEndPointIndex, true);
					}

					//2. remove overlap if necessary
					if(minDelta > 0.0f)
					{ //new minimum point is bigger
						sortMinUp(axis, minEndPointIndex, true);
					}
					if(maxDelta < 0.0f)
					{ //new maximum point is smaller
						sortMaxDown(axis, maxEndPointIndex, true);
					}
				}
			}
		}

		#ifdef _DEBUG
			//printSortedEndPoints();
		#endif
	}

	const std::vector<OverlappingPair *> &SweepAndPrune::getOverlappingPairs() const
	{
		return defaultPairContainer->getOverlappingPairs();
	}

	void SweepAndPrune::createSentinels()
	{
		for(unsigned int axis=0; axis<3; ++axis)
		{
			sortedEndPoints[axis][0].setBodyBox(nullptr); //no box for sentinel
			sortedEndPoints[axis][0].setIsMin(true);
			sortedEndPoints[axis][0].setValue(getMinSentinelValue());

			sortedEndPoints[axis][1].setBodyBox(nullptr); //no box for sentinel
			sortedEndPoints[axis][1].setIsMin(false);
			sortedEndPoints[axis][1].setValue(getMaxSentinelValue());
		}
	}

	float SweepAndPrune::getMinSentinelValue()
	{
		return -std::numeric_limits<float>::max();
	}

	float SweepAndPrune::getMaxSentinelValue()
	{
		return std::numeric_limits<float>::max();
	}

	/**
	 * Sorts the minimum end point for specified axis and adds overlapping pair if necessary.
	 */
	void SweepAndPrune::sortMinDown(unsigned int axis, unsigned int endPointIndex, bool testOverlap)
	{
		BodyEndPoint *endPoint = &sortedEndPoints[axis][endPointIndex];
		BodyEndPoint *prevEndPoint = &sortedEndPoints[axis][endPointIndex-1];

		while(endPoint->getValue() < prevEndPoint->getValue())
		{
			//update indexes in bodies
			if(prevEndPoint->isMin())
			{
				prevEndPoint->getBodyBox()->min[axis]++;
			}else
			{
				const int axis1 = (1  << axis) & 3;
				const int axis2 = (1  << axis1) & 3;
				if(testOverlap && isOverlap(endPoint->getBodyBox(), prevEndPoint->getBodyBox(), axis1, axis2))
				{
					createOverlappingPair(endPoint->getBodyBox(), prevEndPoint->getBodyBox());
				}
				prevEndPoint->getBodyBox()->max[axis]++;
			}
			endPoint->getBodyBox()->min[axis]--;

			//swap end points
			BodyEndPoint tmp = *endPoint;
			*endPoint = *prevEndPoint;
			*prevEndPoint = tmp;

			//decrement
			endPoint--;
			prevEndPoint--;
		}
	}

	/**
	 * Sorts the minimum end point for specified axis and removes overlapping pair if necessary.
	 */
	void SweepAndPrune::sortMaxDown(unsigned int axis, unsigned int endPointIndex, bool testOverlap)
	{
		BodyEndPoint *endPoint = &sortedEndPoints[axis][endPointIndex];
		BodyEndPoint *prevEndPoint = &sortedEndPoints[axis][endPointIndex-1];

		while(endPoint->getValue() < prevEndPoint->getValue())
		{
			//update indexes in bodies
			if(prevEndPoint->isMin())
			{
				const int axis1 = (1  << axis) & 3;
				const int axis2 = (1  << axis1) & 3;
				if(testOverlap && isOverlap(endPoint->getBodyBox(), prevEndPoint->getBodyBox(), axis1, axis2))
				{
					removeOverlappingPair(endPoint->getBodyBox(), prevEndPoint->getBodyBox());
				}
				prevEndPoint->getBodyBox()->min[axis]++;
			}else
			{
				prevEndPoint->getBodyBox()->max[axis]++;
			}
			endPoint->getBodyBox()->max[axis]--;

			//swap end points
			BodyEndPoint tmp = *endPoint;
			*endPoint = *prevEndPoint;
			*prevEndPoint = tmp;

			//decrement
			endPoint--;
			prevEndPoint--;
		}
	}

	/**
	 * Sorts the minimum end point for specified axis and removes overlapping pair if necessary.
	 */
	void SweepAndPrune::sortMinUp(unsigned int axis, unsigned int endPointIndex, bool testOverlap)
	{
		BodyEndPoint *endPoint = &sortedEndPoints[axis][endPointIndex];
		BodyEndPoint *nextEndPoint = &sortedEndPoints[axis][endPointIndex+1];

		while(endPoint->getValue() > nextEndPoint->getValue())
		{
			//update indexes in bodies
			if(nextEndPoint->isMin())
			{
				nextEndPoint->getBodyBox()->min[axis]--;
			}else
			{
				const int axis1 = (1  << axis) & 3;
				const int axis2 = (1  << axis1) & 3;
				if(testOverlap && isOverlap(endPoint->getBodyBox(), nextEndPoint->getBodyBox(), axis1, axis2))
				{
					removeOverlappingPair(endPoint->getBodyBox(), nextEndPoint->getBodyBox());
				}
				nextEndPoint->getBodyBox()->max[axis]--;
			}
			endPoint->getBodyBox()->min[axis]++;

			//swap end points
			BodyEndPoint tmp = *endPoint;
			*endPoint = *nextEndPoint;
			*nextEndPoint = tmp;

			//increment
			endPoint++;
			nextEndPoint++;
		}
	}

	/**
	 * Sorts the maximum end point for specified axis and adds overlapping pair if necessary.
	 */
	void SweepAndPrune::sortMaxUp(unsigned int axis, unsigned int endPointIndex, bool testOverlap)
	{
		BodyEndPoint *endPoint = &sortedEndPoints[axis][endPointIndex];
		BodyEndPoint *nextEndPoint = &sortedEndPoints[axis][endPointIndex+1];

		while(endPoint->getValue() > nextEndPoint->getValue())
		{
			//update indexes in bodies
			if(nextEndPoint->isMin())
			{
				const int axis1 = (1  << axis) & 3;
				const int axis2 = (1  << axis1) & 3;
				if(testOverlap && isOverlap(endPoint->getBodyBox(), nextEndPoint->getBodyBox(), axis1, axis2))
				{
					createOverlappingPair(endPoint->getBodyBox(), nextEndPoint->getBodyBox());
				}
				nextEndPoint->getBodyBox()->min[axis]--;
			}else
			{
				nextEndPoint->getBodyBox()->max[axis]--;
			}
			endPoint->getBodyBox()->max[axis]++;

			//swap end points
			BodyEndPoint tmp = *endPoint;
			*endPoint = *nextEndPoint;
			*nextEndPoint = tmp;

			//increment
			endPoint++;
			nextEndPoint++;
		}
	}

	/**
	 * Tests if both bodies overlap on specified axis
	 */
	bool SweepAndPrune::isOverlap(const BodyBox *body1, const BodyBox *body2, unsigned int axis1, unsigned int axis2)
	{
		//comparing indexes is more faster that values
		if (body1->max[axis1] < body2->min[axis1] ||
				body2->max[axis1] < body1->min[axis1] ||
				body1->max[axis2] < body2->min[axis2] ||
				body2->max[axis2] < body1->min[axis2])
		{
			return false;
		}
		return true;
	}

	void SweepAndPrune::createOverlappingPair(const BodyBox *bodyBox1, const BodyBox *bodyBox2)
	{
		if(!bodyBox1->hasAlternativePairContainer() && !bodyBox2->hasAlternativePairContainer())
		{
			defaultPairContainer->addOverlappingPair(bodyBox1->getOwner(), bodyBox2->getOwner());
		}else
		{
			if(bodyBox1->hasAlternativePairContainer())
			{
				bodyBox1->getAlternativePairContainer()->addOverlappingPair(bodyBox1->getOwner(), bodyBox2->getOwner());
			}

			if(bodyBox2->hasAlternativePairContainer())
			{
				bodyBox2->getAlternativePairContainer()->addOverlappingPair(bodyBox1->getOwner(), bodyBox2->getOwner());
			}
		}
	}

	void SweepAndPrune::removeOverlappingPair(const BodyBox *bodyBox1, const BodyBox *bodyBox2)
	{
		if(!bodyBox1->hasAlternativePairContainer() && !bodyBox2->hasAlternativePairContainer())
		{
			defaultPairContainer->removeOverlappingPair(bodyBox1->getOwner(), bodyBox2->getOwner());
		}else
		{
			if(bodyBox1->hasAlternativePairContainer())
			{
				bodyBox1->getAlternativePairContainer()->removeOverlappingPair(bodyBox1->getOwner(), bodyBox2->getOwner());
			}

			if(bodyBox2->hasAlternativePairContainer())
			{
				bodyBox2->getAlternativePairContainer()->removeOverlappingPair(bodyBox1->getOwner(), bodyBox2->getOwner());
			}
		}
	}

#ifdef _DEBUG
	void SweepAndPrune::printSortedEndPoints()
	{
		unsigned int limit = bodiesBox.size() * 2 + 2; //bodies size * both points (min & max) + 2 sentinels
		for(unsigned int axis=0; axis<3; ++axis)
		{
			std::cout<<"Axis "<<axis<<":"<<std::endl;
			for(unsigned int i=0; i<limit; ++i)
			{
				std::string idBody = "_sentinel";
				BodyBox *bodyBox = sortedEndPoints[axis][i].getBodyBox();
				if(bodyBox!=nullptr)
				{
					idBody = bodyBox->getOwner()->getId();
				}

				std::cout<<" - ["<<idBody<<", "<<sortedEndPoints[axis][i].getValue()<<"]"<<std::endl;
			}
			std::cout<<std::endl;
		}
		std::cout<<std::endl;
	}
#endif

}
