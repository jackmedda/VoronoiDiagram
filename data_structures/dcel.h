#ifndef DCEL_H
#define DCEL_H

#include <vector>

#include "vertex.h"
#include "half_edge.h"
#include "face.h"

namespace Voronoi {

/**
  * @class DCEL
  * @brief The DCEL class models a doubly connected edge list
*/
    class DCEL {
        public:
            DCEL();
        private:
            std::vector<Vertex> vertexs;
            std::vector<HalfEdge> half_edges;
            std::vector<Face> faces;
    };
}

#endif // DCEL_H
