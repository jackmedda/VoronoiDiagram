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
            Vertex(const cg3::Point2Dd& coordinates);
            Vertex(const cg3::Point2Dd& coordinates, const size_t incidEdge);

            const cg3::Point2Dd& getCoordinates() const;
            size_t getIncidEdgeID() const;
            void setIncidEdge(const size_t incidEdge);
            void setCoordinates(const cg3::Point2Dd& p);
    private:
            cg3::Point2Dd _coordinates;
            size_t _incidEdge;
    };

}

#include "vertex.tpp"

#endif // VERTEX_H
