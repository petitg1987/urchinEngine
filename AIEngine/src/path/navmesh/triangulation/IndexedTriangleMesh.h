#ifndef URCHINENGINE_INDEXEDTRIANGLEMESH_H
#define URCHINENGINE_INDEXEDTRIANGLEMESH_H

namespace urchin
{

    class IndexedTriangleMesh
    {
        public:
            IndexedTriangleMesh(unsigned int, unsigned int, unsigned int);

            const unsigned int *getIndices() const;
            unsigned int getIndex(unsigned int) const;

            void addNeighbor(unsigned int, int);
            int getNeighbor(unsigned int) const;

        private:
            unsigned int indices[3];
            int neighbors[3];
    };

}

#endif
