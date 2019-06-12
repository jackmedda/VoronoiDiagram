#include "vertex.h"

namespace Voronoi {

inline Vertex::Vertex() : _coordinates(), _indicEdge() {
}

inline Vertex::Vertex(const cg3::Point2Dd& coordinates, size_t indicEdge) :
    _coordinates(coordinates), _indicEdge(indicEdge) {
}

inline const cg3::Point2Dd& Vertex::getCoordinates() const {
    return _coordinates;
}

inline size_t Vertex::getIncidEdgeID() const {
    return _indicEdge;
}

void Vertex::setIndicEdge(size_t indicEdge)
{
    _indicEdge = indicEdge;
}

}
