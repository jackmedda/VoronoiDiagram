#ifndef HALF_EDGE_H
#define HALF_EDGE_H

#include <stddef.h> //for size_t
#include <limits>

namespace Voronoi {
    /**
     * @class Half_Edge
     * @brief The HalfEdge class
     */
    class HalfEdge {
        public:
            HalfEdge();
            HalfEdge(size_t origin, size_t twin,
                     size_t next, size_t prev);

            size_t getOriginID() const;
            size_t getTwinID() const;
            size_t getNextID() const;
            size_t getPrevID() const;
            void setOrigin(size_t origin);
            void setTwin(size_t twin);
            void setNext(size_t next);
            void setPrev(size_t prev);

        private:
            size_t _origin;
            size_t _twin;
            size_t _next;
            size_t _prev;
    };

}

#include "half_edge.tpp"

#endif // HALF_EDGE_H
