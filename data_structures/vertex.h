#ifndef VERTEX_H
#define VERTEX_H

#include <vector>

#include <cg3/geometry/2d/point2d.h>

namespace Voronoi {
    /**
     * Forward declaration of HalfEdge
     */
    class HalfEdge;
    /**
     * @class Vertex
     * @brief The Vertex class models a DCEL Vertex
     */
    class Vertex {
        public:
            Vertex();
            Vertex(const cg3::Point2D<float>& coordinates, const Voronoi::HalfEdge& indicEdge);

            const cg3::Point2D<float>& getCoordinates() const;
            const Voronoi::HalfEdge& getIndicEdge() const;
            void setIndicEdge(const Voronoi::HalfEdge& indicEdge);
    private:
            cg3::Point2D<float> _coordinates;
            const Voronoi::HalfEdge* _indicEdge;
    };

}

#endif // VERTEX_H
