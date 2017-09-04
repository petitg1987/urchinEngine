#include <cassert>

#include "IndexedTriangleMesh.h"

namespace urchin
{

    IndexedTriangleMesh::IndexedTriangleMesh(unsigned int index1, unsigned int index2, unsigned int index3)
    {
        #ifdef _DEBUG
            assert(index1!=index2 && index1!=index3 && index2!=index3);
        #endif

        this->indices[0] = index1;
        this->indices[1] = index2;
        this->indices[2] = index3;

        this->neighbors[0] = -1;
        this->neighbors[1] = -1;
        this->neighbors[2] = -1;
    }

    const unsigned int *IndexedTriangleMesh::getIndices() const
    {
        return indices;
    }

    unsigned int IndexedTriangleMesh::getIndex(unsigned int index) const
    {
        #ifdef _DEBUG
            assert(index <= 2);
        #endif

        return indices[index];
    }

    void IndexedTriangleMesh::addNeighbor(unsigned int edgeIndex, int triangleNeighborIndex)
    {
        #ifdef _DEBUG
            assert(edgeIndex >= 0 && edgeIndex <= 2);
            assert(neighbors[edgeIndex]==-1); //check neighbor not already provided
            assert(triangleNeighborIndex >= 0);
        #endif

        neighbors[edgeIndex] = triangleNeighborIndex;
    }

    int IndexedTriangleMesh::getNeighbor(unsigned int edgeIndex) const
    {
        #ifdef _DEBUG
            assert(edgeIndex <= 2);
        #endif

        return neighbors[edgeIndex];
    }

}
