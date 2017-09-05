#ifndef URCHINENGINE_NAVTRIANGLE_H
#define URCHINENGINE_NAVTRIANGLE_H

namespace urchin
{

    class NavTriangle
    {
        public:
            NavTriangle(unsigned int, unsigned int, unsigned int);

            const unsigned int *getIndices() const;
            unsigned int getIndex(unsigned int) const;

            void addNeighbor(unsigned int, int);
            const int *getNeighbors() const;
            int getNeighbor(unsigned int) const;

        private:
            unsigned int indices[3];
            int neighbors[3];
    };

}

#endif
