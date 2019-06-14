#include "vertex.h"

namespace Voronoi {

inline Vertex::Vertex() : _coordinates(), _incidEdge() {
}

inline Vertex::Vertex(const cg3::Point2Dd& coordinates, size_t incidEdge) :
    _coordinates(coordinates), _incidEdge(incidEdge) {
}

inline const cg3::Point2Dd& Vertex::getCoordinates() const {
    return _coordinates;
}

inline size_t Vertex::getIncidEdgeID() const {
    return _incidEdge;
}

inline void Vertex::setIncidEdge(size_t incidEdge) {
    _incidEdge = incidEdge;
}

}
