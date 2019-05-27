#ifndef HALF_EDGE_H
#define HALF_EDGE_H

namespace Voronoi {
    /**
     * Forward declaration of Vertex and Face
     */
    class Vertex;
    //class Face;
    /**
     * @class Half_Edge
     * @brief The HalfEdge class
     */
    class HalfEdge {
        public:
            HalfEdge();
            HalfEdge(const Voronoi::Vertex& origin, const Voronoi::HalfEdge& twin, /*const Voronoi::Face& incidFace,*/
                     const Voronoi::HalfEdge& next, const Voronoi::HalfEdge& prev);

            const Voronoi::Vertex& getOrigin() const;
            const Voronoi::HalfEdge& getTwin() const;
            //const Voronoi::Face& getIncidFace() const;
            const Voronoi::HalfEdge& getNext() const;
            const Voronoi::HalfEdge& getPrev() const;
            void setOrigin(const Voronoi::Vertex& origin);

    private:
            const Voronoi::Vertex* _origin;
            const Voronoi::HalfEdge* _twin;
            //const Voronoi::Face* _incidFace;
            const Voronoi::HalfEdge* _next;
            const Voronoi::HalfEdge* _prev;
    };

}
#endif // HALF_EDGE_H
