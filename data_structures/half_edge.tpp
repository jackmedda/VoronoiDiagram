#include "half_edge.h"

namespace Voronoi {
//IDs are set to the maximum value that a size_t could contain, it represents an "empty" reference
inline HalfEdge::HalfEdge() : _origin(std::numeric_limits<size_t>::max()), _twin(std::numeric_limits<size_t>::max()),
    _next(std::numeric_limits<size_t>::max()), _prev(std::numeric_limits<size_t>::max()) {
}

inline HalfEdge::HalfEdge(size_t origin, size_t twin, size_t next, size_t prev) :
                          _origin(origin), _twin(twin), _next(next), _prev(prev) {
}

inline size_t HalfEdge::getOriginID() const {
    return _origin;
}

inline size_t HalfEdge::getTwinID() const {
    return _twin;
}

inline size_t HalfEdge::getNextID() const {
    return _next;
}

inline size_t HalfEdge::getPrevID() const {
    return _prev;
}

inline void HalfEdge::setOrigin(size_t origin) {
    _origin = origin;
}

inline void HalfEdge::setTwin(size_t twin) {
    _twin = twin;
}

inline void HalfEdge::setNext(size_t next) {
    _next = next;
}

inline void HalfEdge::setPrev(size_t prev) {
    _prev = prev;
}

}
