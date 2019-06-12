#include "dcel.h"

namespace Voronoi {
    /**
     * @brief DCEL::addVertex
     * @param V: vertex to add to the DCEL
     * @return the index where the vertex is stored
     */
    size_t DCEL::addVertex(Voronoi::Vertex &V) {
        vertexs.push_back(V);
        return vertexs.size()-1;
    }

    /**
     * @brief DCEL::addHalfEdge
     * @param HE: halfEdge to add to the DCEL
     * @return the index where the halfEdge is stored
     */
    size_t DCEL::addHalfEdge(Voronoi::HalfEdge &HE) {
        halfEdges.push_back(HE);
        return  halfEdges.size()-1;
    }
}
