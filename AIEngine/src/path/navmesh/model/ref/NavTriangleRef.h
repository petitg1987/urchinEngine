#ifndef URCHINENGINE_NAVTRIANGLEREF_H
#define URCHINENGINE_NAVTRIANGLEREF_H

namespace urchin
{

    class NavTriangleRef
    {
        public:
            NavTriangleRef(unsigned int, unsigned int);

            unsigned int getPolygonIndex() const;
            unsigned int getTriangleIndex() const;

            bool equals(const NavTriangleRef &) const;

        private:
            unsigned int polygonIndex;
            unsigned int triangleIndex;
    };

}

#endif
