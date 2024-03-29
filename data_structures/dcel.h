#ifndef DCEL_H
#define DCEL_H

#include <vector>

#include "vertex.h"
#include "half_edge.h"

namespace Voronoi {

    /**
      * @class DCEL
      * @brief The DCEL class models a doubly connected edge list
    */
    class DCEL {
        public:
            DCEL() = default;

            void clear();

            //vertexs methods
            std::vector<Vertex>& getVertexs();
            const Voronoi::HalfEdge& getIncidEdge(size_t vertexIndex) const;
            size_t addVertex(const Voronoi::Vertex& V);

            //halfedges methods
            std::vector<HalfEdge>& getHalfEdges();
            const Voronoi::Vertex& getHEOrigin(size_t halfEdgeIndex) const;
            const Voronoi::HalfEdge& getHETwin(size_t halfEdgeIndex) const;
            const Voronoi::HalfEdge& getHENext(size_t halfEdgeIndex) const;
            const Voronoi::HalfEdge& getHEPrev(size_t halfEdgeIndex) const;
            size_t addHalfEdge(const Voronoi::HalfEdge& HE);
        protected:
            std::vector<Vertex> vertexs;
            std::vector<HalfEdge> halfEdges;
    };

    inline void DCEL::clear() {
        vertexs.clear();
        halfEdges.clear();
    }

    inline std::vector<Vertex>& DCEL::getVertexs() {
        return vertexs;
    }

    inline const Voronoi::HalfEdge& DCEL::getIncidEdge(size_t vertexIndex) const {
        return halfEdges[vertexs[vertexIndex].getIncidEdgeID()];
    }

    inline std::vector<HalfEdge>& DCEL::getHalfEdges() {
        return halfEdges;
    }

    inline const Voronoi::Vertex& DCEL::getHEOrigin(size_t halfEdgeIndex) const {
        return vertexs[halfEdges[halfEdgeIndex].getOriginID()];
    }

    inline const Voronoi::HalfEdge& DCEL::getHETwin(size_t halfEdgeIndex) const {
        return halfEdges[halfEdges[halfEdgeIndex].getTwinID()];
    }

    inline const Voronoi::HalfEdge& DCEL::getHENext(size_t halfEdgeIndex) const {
        return halfEdges[halfEdges[halfEdgeIndex].getNextID()];
    }

    inline const Voronoi::HalfEdge& DCEL::getHEPrev(size_t halfEdgeIndex) const {
        return halfEdges[halfEdges[halfEdgeIndex].getPrevID()];
    }
}

#endif // DCEL_H
