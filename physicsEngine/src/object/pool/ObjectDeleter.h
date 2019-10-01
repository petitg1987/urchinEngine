#ifndef URCHINENGINE_OBJECTDELETER_H
#define URCHINENGINE_OBJECTDELETER_H

namespace urchin
{

    class CollisionConvexObject3D;

    class ObjectDeleter
    {
        public:
            void operator()(CollisionConvexObject3D *);
    };

}

#endif
