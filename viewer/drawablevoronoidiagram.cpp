#include "drawablevoronoidiagram.h"

#include "cg3/viewer/renderable_objects/2d/renderable_objects2d.h"

namespace Voronoi {

    DrawableVoronoiDiagram::DrawableVoronoiDiagram() : pointSize(2), pointColor(Qt::red), edgeWidth(1), edgeColor(){}

    void DrawableVoronoiDiagram::draw() const {
        drawEdges();
        drawPoints();
    }

    cg3::Pointd DrawableVoronoiDiagram::sceneCenter() const {
        cg3::Pointd center(0,0,0);
        for(const Vertex& v : vertexs) {
            center.x() += v.getCoordinates().x();
            center.y() += v.getCoordinates().y();
        }
        center /= vertexs.size();
        return center;
    }

    double DrawableVoronoiDiagram::sceneRadius() const {
        double maxDist = 0, dist;
        cg3::Pointd center = sceneCenter();
        for(const Vertex& v : vertexs) {
            dist = v.getCoordinates().dist(cg3::Point2Dd(center.x(), center.y()));
            maxDist = std::max(dist, maxDist);
        }

        return maxDist;
    }

    void DrawableVoronoiDiagram::drawEdges() const {
        for(size_t i = 0; i < halfEdges.size(); i++) {
            if(halfEdges[i].getOriginID() != std::numeric_limits<size_t>::max() &&
                    halfEdges[halfEdges[i].getTwinID()].getOriginID() != std::numeric_limits<size_t>::max())
                cg3::viewer::drawLine2D(getHEOrigin(i).getCoordinates(), getHEOrigin(halfEdges[i].getTwinID()).getCoordinates(),
                                    edgeColor, edgeWidth);
        }
    }

    void DrawableVoronoiDiagram::drawPoints() const {
        for(const Vertex& v : vertexs) {
            if(v.getIncidEdgeID() != std::numeric_limits<size_t>::max())
                cg3::viewer::drawPoint2D(v.getCoordinates(), pointColor, pointSize);
        }
    }

}
