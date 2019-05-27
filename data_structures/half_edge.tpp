#include "half_edge.h"

namespace Voronoi {
inline HalfEdge::HalfEdge() : _origin(), _twin(), /*_incidFace(),*/ _next(), _prev() {
}

inline HalfEdge::HalfEdge(const Voronoi::Vertex& origin, const Voronoi::HalfEdge& twin, /*const Voronoi::Face& incidFace,*/
                          const Voronoi::HalfEdge& next, const Voronoi::HalfEdge& prev) :
                          _origin(&origin), _twin(&twin), /*_incidFace(&incidFace),*/ _next(&next), _prev(&prev) {
}

inline const Voronoi::Vertex& HalfEdge::getOrigin() const {
    return *_origin;
}

inline const Voronoi::HalfEdge& HalfEdge::getTwin() const {
    return *_twin;
}

/*inline const Voronoi::Face& HalfEdge::getIncidFace() const {
    return *_incidFace;
}*/

inline const Voronoi::HalfEdge& HalfEdge::getNext() const {
    return *_next;
}

inline const Voronoi::HalfEdge& HalfEdge::getPrev() const {
    return *_prev;
}

inline void HalfEdge::setOrigin(const Voronoi::Vertex& origin)
{
    _origin = &origin;
}

}
