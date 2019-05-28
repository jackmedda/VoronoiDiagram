#ifndef FACE_H
#define FACE_H

namespace Voronoi {
    /**
     * Forward declaration of HalfEdge
     */
    class HalfEdge;
    /**
     * @class Face
     * @brief The Face class
     */
    class Face
    {
        public:
            Face();
            Face(const Voronoi::HalfEdge& outerComponent, const Voronoi::HalfEdge& innerComponent);

            const Voronoi::HalfEdge& getOuterComponent() const;
            const Voronoi::HalfEdge& getInnerComponent() const;
            void setOuterComponent(const Voronoi::HalfEdge& outerComponent);
            void setInnerComponent(const Voronoi::HalfEdge& innerComponent);
    private:
            const Voronoi::HalfEdge* _outerComponent;
            const Voronoi::HalfEdge* _innerComponent;
    };

}

#include "face.tpp"

#endif // FACE_H
