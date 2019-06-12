#ifndef VERTEX_H
#define VERTEX_H

#include <cg3/geometry/2d/point2d.h>

namespace Voronoi {
    /**
     * @class Vertex
     * @brief The Vertex class models a DCEL Vertex
     */
    class Vertex {
        public:
            Vertex();
            Vertex(const cg3::Point2Dd& coordinates, size_t indicEdge);

            const cg3::Point2Dd& getCoordinates() const;
            size_t getIncidEdgeID() const;
            void setIndicEdge(size_t indicEdge);
    private:
            cg3::Point2Dd _coordinates;
            size_t _indicEdge;
    };

}

#include "vertex.tpp"

#endif // VERTEX_H
