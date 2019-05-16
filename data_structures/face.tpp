#include "face.h"

namespace Voronoi {

inline Face::Face() : _outerComponent(), _innerComponent()  {
}

inline Face::Face(const Voronoi::HalfEdge& outerComponent, const Voronoi::HalfEdge& innerComponent) :
    _outerComponent(&outerComponent), _innerComponent(&innerComponent){
}

inline const Voronoi::HalfEdge& Face::getOuterComponent() const {
    return *_outerComponent;
}

inline const Voronoi::HalfEdge& Face::getInnerComponent() const {
    return *_innerComponent;
}

inline void Face::setOuterComponent(const Voronoi::HalfEdge& outerComponent)
{
_outerComponent = &outerComponent;
}

inline void Face::setInnerComponent(const Voronoi::HalfEdge& innerComponent)
{
_innerComponent = &innerComponent;
}

}
