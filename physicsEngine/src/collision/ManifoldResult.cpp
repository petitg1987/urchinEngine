#include <cassert>

#include "collision/ManifoldResult.h"

namespace urchin
{

	ManifoldResult::ManifoldResult(AbstractWorkBody *body1, AbstractWorkBody *body2) :
			body1(body1),
			body2(body2),
			nbContactPoint(0),
			contactBreakingThreshold(ConfigService::instance()->getFloatValue("narrowPhase.contactBreakingThreshold"))
	{

	}

	ManifoldResult::~ManifoldResult()
	{

	}

	/**
	 * @param index Index of body to return. Index must be '0' for body1 or '1' for body2.
	 */
	AbstractWorkBody *ManifoldResult::getBody(unsigned int index) const
	{
		#ifdef _DEBUG
			assert(index==0 || index==1);
		#endif

		return index==0 ? body1 : body2;
	}

	AbstractWorkBody *ManifoldResult::getBody1() const
	{
		return body1;
	}

	AbstractWorkBody *ManifoldResult::getBody2() const
	{
		return body2;
	}

	unsigned int ManifoldResult::getNumContactPoints() const
	{
		return nbContactPoint;
	}

	float ManifoldResult::getContactBreakingThreshold() const
	{
		return contactBreakingThreshold;
	}

	ManifoldContactPoint &ManifoldResult::getManifoldContactPoint(unsigned int index)
	{
		return contactPoints[index];
	}

	const ManifoldContactPoint &ManifoldResult::getManifoldContactPoint(unsigned int index) const
	{
		return contactPoints[index];
	}

	/**
	 * @param normalFromObject2 Contact normal from object 2. The normal direction should be toward the object 1.
	 * @param pointOnObject2 Contact point on object 2 in world coordinate
	 * @param depth Penetration depth (negative when collision exist)
	 * @param isPredictive Is a predictive contact point
	 */
	void ManifoldResult::addContactPoint(const Vector3<float> &normalFromObject2, const Point3<float> &pointOnObject2, float depth, bool isPredictive)
	{
		Point3<float> pointOnObject1 = pointOnObject2.translate(normalFromObject2 * depth);
		Point3<float> localPointOnObject1 = body1->getPhysicsTransform().inverseTransform(pointOnObject1);
		Point3<float> localPointOnObject2 = body2->getPhysicsTransform().inverseTransform(pointOnObject2);

		addContactPoint(normalFromObject2, pointOnObject1, pointOnObject2, localPointOnObject1, localPointOnObject2, depth, isPredictive);
	}

	/**
	 * @param normalFromObject2 Contact normal from object 2. The normal direction should be toward the object 1.
	 * @param pointOnObject1 Contact point on object 1 in world coordinate
	 * @param pointOnObject2 Contact point on object 2 in world coordinate
	 * @param localPointOnObject1 Contact point on object 1 in local coordinate
	 * @param localPointOnObject2 Contact point on object 2 in local coordinate
	 * @param depth Penetration depth (negative when collision exist)
	 * @param isPredictive Is a predictive contact point
	 */
	void ManifoldResult::addContactPoint(const Vector3<float> &normalFromObject2, const Point3<float> &pointOnObject1, const Point3<float> &pointOnObject2,
			const Point3<float> &localPointOnObject1, const Point3<float> &localPointOnObject2, float depth, bool isPredictive)
	{
		#ifdef _DEBUG
			if(!isPredictive)
			{
				assert(depth <= contactBreakingThreshold);
			}
		#endif

		//1. if similar point exist in manifold result: replace it
		int nearestPointIndex = getNearestPointIndex(localPointOnObject2);
		if(nearestPointIndex >= 0)
		{ //replace existing point
			contactPoints[nearestPointIndex] = ManifoldContactPoint(normalFromObject2, pointOnObject1, pointOnObject2,
					localPointOnObject1, localPointOnObject2, depth, isPredictive);
			return;
		}

		//2. keep deepest point and four points cover the biggest area
		unsigned int insertionIndex;
		if(nbContactPoint==MAX_PERSISTENT_POINTS)
		{
			insertionIndex = computeBestInsertionIndex(localPointOnObject2);
		}else
		{
			insertionIndex = nbContactPoint;
			++nbContactPoint;
		}
		contactPoints[insertionIndex] = ManifoldContactPoint(normalFromObject2, pointOnObject1, pointOnObject2,
				localPointOnObject1, localPointOnObject2, depth, isPredictive);
	}

	void ManifoldResult::refreshContactPoints()
	{
		for(unsigned int i=0; i<nbContactPoint; ++i)
		{
			//1. update contact points in world position
			Point3<float> newContactPointOnObject1 = body1->getPhysicsTransform().transform(contactPoints[i].getLocalPointOnObject1());
			Point3<float> newContactPointOnObject2 = body2->getPhysicsTransform().transform(contactPoints[i].getLocalPointOnObject2());
			contactPoints[i].updatePoints(newContactPointOnObject1, newContactPointOnObject2);

			//2. update penetration depth
			float newDepth = (newContactPointOnObject2.vector(newContactPointOnObject1)).dotProduct(contactPoints[i].getNormalFromObject2());
			contactPoints[i].updateDepth(newDepth);
		}

		for(int i=static_cast<int>(nbContactPoint-1); i>=0; --i)
		{ //loop from last to first in order to be able to remove contact points
			if(contactPoints[i].getDepth() > contactBreakingThreshold)
			{
				removeContactPoint(i);
			}else
			{
				Point3<float> projectedPointOnObject2 = contactPoints[i].getPointOnObject1().translate(-(contactPoints[i].getNormalFromObject2() * contactPoints[i].getDepth()));
				Vector3<float> projectedDifference = projectedPointOnObject2.vector(contactPoints[i].getPointOnObject2());
				if(projectedDifference.squareLength() > contactBreakingThreshold * contactBreakingThreshold)
				{
					removeContactPoint(i);
				}
			}
		}
	}

	/**
	 * @param localPointOnObject2 Local point of object 2 used for comparison
	 * @return Nearest point index to point given in parameter. If all points are too far: '-1' is returned.
	 */
	int ManifoldResult::getNearestPointIndex(const Point3<float> &localPointOnObject2) const
	{
		float shortestDistance = contactBreakingThreshold * contactBreakingThreshold;
		int nearestPointIndex = -1;
		for(unsigned int i=0; i<nbContactPoint; ++i)
		{
			Vector3<float> diff = localPointOnObject2.vector(contactPoints[i].getLocalPointOnObject2());
			float diffSquareLength = diff.squareLength();
			if(diffSquareLength < shortestDistance)
			{
				shortestDistance = diffSquareLength;
				nearestPointIndex = static_cast<int>(i);
			}
		}

		return nearestPointIndex;
	}

	/**
	 * Compute the best index where to insert the new point when MAX_PERSISTENT_POINTS is reach.
	 * We keep deepest point and four points cover the biggest area.
	 * @param localPointOnObject2 Local point of object 2
	 * @return Best index to insert the new point
	 */
	unsigned int ManifoldResult::computeBestInsertionIndex(const Point3<float> &localPointOnObject2) const
	{
		assert(nbContactPoint==MAX_PERSISTENT_POINTS);

		unsigned int deepestIndex = getDeepestPointIndex();
		float areas[MAX_PERSISTENT_POINTS]; //areas[X] contains area formed by all points except point 'contactPoints[X]'
		for(unsigned int i=0; i<MAX_PERSISTENT_POINTS; ++i)
		{
			areas[i] = 0.0;
		}

		//compute areas
		if(deepestIndex != 0)
		{
			areas[0] = computeArea(localPointOnObject2, contactPoints[1].getLocalPointOnObject2(),
					contactPoints[2].getLocalPointOnObject2(), contactPoints[3].getLocalPointOnObject2());
		}

		if(deepestIndex != 1)
		{
			areas[1] = computeArea(localPointOnObject2, contactPoints[0].getLocalPointOnObject2(),
					contactPoints[2].getLocalPointOnObject2(), contactPoints[3].getLocalPointOnObject2());
		}

		if(deepestIndex != 2)
		{
			areas[2] = computeArea(localPointOnObject2, contactPoints[0].getLocalPointOnObject2(),
					contactPoints[1].getLocalPointOnObject2(), contactPoints[3].getLocalPointOnObject2());
		}

		if(deepestIndex != 3)
		{
			areas[3] = computeArea(localPointOnObject2, contactPoints[0].getLocalPointOnObject2(),
					contactPoints[1].getLocalPointOnObject2(), contactPoints[2].getLocalPointOnObject2());
		}

		//find biggest area
		unsigned int insertionIndex = 0;
		float biggestArea = areas[insertionIndex];
		for(unsigned int i=1; i<MAX_PERSISTENT_POINTS; ++i)
		{
			if(areas[i] > biggestArea)
			{
				biggestArea = areas[i];
				insertionIndex = i;
			}
		}

		return insertionIndex;
	}

	unsigned int ManifoldResult::getDeepestPointIndex() const
	{
		float deepestValue = contactPoints[0].getDepth();
		unsigned int deepestIndex = 0;
		for(unsigned int i=1; i<nbContactPoint; ++i)
		{
			if(contactPoints[i].getDepth() < deepestValue)
			{
				deepestValue = contactPoints[i].getDepth();
				deepestIndex = i;
			}
		}

		return deepestIndex;
	}

	/**
	 * Computes area formed by the four points. Formula used: max(|v1.crossProduct(v2)|²).
	 */
	float ManifoldResult::computeArea(const Point3<float> &p0, const Point3<float> &p1, const Point3<float> &p2, const Point3<float> &p3) const
	{
		Vector3<float> vectorP1p0 = p1.vector(p0);
		Vector3<float> vectorP2p0 = p2.vector(p0);
		Vector3<float> vectorP3p0 = p3.vector(p0);
		Vector3<float> vectorP3p2 = p3.vector(p2);
		Vector3<float> vectorP3p1 = p3.vector(p1);
		Vector3<float> vectorP2p1 = p2.vector(p1);

		Vector3<float> p1p0CrossP3p2 = vectorP1p0.crossProduct(vectorP3p2);
		Vector3<float> p2p0CrossP3p1 = vectorP2p0.crossProduct(vectorP3p1);
		Vector3<float> p3p0CrossP2p1 = vectorP3p0.crossProduct(vectorP2p1);

		return std::max(std::max(p1p0CrossP3p2.squareLength(), p2p0CrossP3p1.squareLength()), p3p0CrossP2p1.squareLength());
	}

	void ManifoldResult::removeContactPoint(unsigned int index)
	{
		unsigned int lastUsedIndex = getNumContactPoints() - 1;
		if(index != lastUsedIndex)
		{//copy the last contact point on the one we remove
			contactPoints[index] = contactPoints[lastUsedIndex];
		}

		--nbContactPoint;
	}

}
