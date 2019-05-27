#include "vertex.h"

namespace Voronoi {

inline Vertex::Vertex() : _coordinates(), _indicEdge() {
}

inline Vertex::Vertex(const cg3::Point2Dd& coordinates, const Voronoi::HalfEdge& indicEdge) :
    _coordinates(coordinates), _indicEdge(&indicEdge) {
}

inline const cg3::Point2Dd& Vertex::getCoordinates() const {
    return _coordinates;
}

inline const Voronoi::HalfEdge& Vertex::getIndicEdge() const {
    return *_indicEdge;
}

void Vertex::setIndicEdge(const Voronoi::HalfEdge& indicEdge)
{
    _indicEdge = &indicEdge;
}

}
