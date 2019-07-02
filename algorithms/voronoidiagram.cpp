#include "voronoidiagram.h"

namespace Voronoi {

    void checkCircleEvent(const Leaf* l1, Leaf* middleArc, const Leaf* l3, const double& sweepline,
                          std::priority_queue<Event*, std::vector<Event*>, EventComparator>& pq) {

        //if newPoint->next->next or newPoint->prev->prev is null
        if(!l3)
            return;

        cg3::Point2Dd center, circleEventPoint;

        if(!findCircleCenter(*(l1->site), *(middleArc->site), *(l3->site), center))
            return;

        circleEventPoint.set(center.x(), center.y() - center.dist(*(l1->site)));
        if((circleEventPoint.y() - sweepline) < POINT_EPSILON || circleEventPoint.y() < sweepline) {
            CircleEvent* e = new CircleEvent(circleEventPoint, center, middleArc);
            middleArc->circleEvent = e;
            pq.push(e);
        }
    }

    void fortuneAlgorithm(const std::vector<cg3::Point2Dd>& points, DrawableVoronoiDiagram& dcel,
                          const cg3::DrawableBoundingBox2D& boundingBox) {
        double sweepline;
        Beachline beachline(&sweepline);
        std::priority_queue<Event*, std::vector<Event*>, EventComparator> pq;

        for(const cg3::Point2Dd& p : points) {
            pq.push(static_cast<Event*>(new SiteEvent(p)));
        }

        while(!pq.empty()) {
            Event* e = pq.top();
            pq.pop();
            sweepline = e->getPoint().y();

            if(e->getType() == Event::EventType::SITE) {
                Leaf* newPoint = nullptr;
                CircleEvent* oldPointEvent = beachline.addPoint(e->getPoint(), newPoint, dcel.getHalfEdges());

                if(newPoint) {//if it is nullptr, the beachline was empty before the addPoint
                    if(oldPointEvent)
                        oldPointEvent->setType(Event::EventType::FALSE);

                    checkCircleEvent(newPoint, newPoint->next, newPoint->next->next, sweepline, pq);
                    checkCircleEvent(newPoint, newPoint->prev, newPoint->prev->prev, sweepline, pq);
                }
            } else if(e->getType() == Event::EventType::CIRCLE) {
                CircleEvent* cE = static_cast<CircleEvent*>(e);
                Leaf* prev = beachline.removePoint(cE, dcel);

                //If null it is a false alarm
                if(!prev)
                    continue;

                //Saving prev and next pointers of the arc that is going to be removed
                prev->next->next->prev = prev;
                prev->next = prev->next->next;

                //Delete circle events involving arc
                if(prev->circleEvent) {
                    prev->circleEvent->setType(Event::EventType::FALSE);
                    prev->circleEvent = nullptr;
                }
                if(prev->next->circleEvent) {
                    prev->next->circleEvent->setType(Event::EventType::FALSE);
                    prev->next->circleEvent = nullptr;
                }

                checkCircleEvent(prev->next, prev, prev->prev, sweepline, pq);
                checkCircleEvent(prev, prev->next, prev->next->next, sweepline, pq);
            }

            delete e;
        }

        //Connect halfedges to the bounding box
        std::vector<HalfEdge>& edges = dcel.getHalfEdges();
        std::vector<Vertex>& vertexs = dcel.getVertexs();
        for(size_t i=0; i < edges.size(); i++) {
            if(edges[i].getOriginID() != std::numeric_limits<size_t>::max()) {
                if(!boundingBox.isInside(dcel.getHEOrigin(i).getCoordinates())) {
                    if(edges[edges[i].getTwinID()].getOriginID() != std::numeric_limits<size_t>::max()) {
                        const cg3::Point2Dd& vIn = vertexs[edges[edges[i].getTwinID()].getOriginID()].getCoordinates();
                        size_t index = edges[i].getOriginID();
                        const cg3::Point2Dd& vOut = vertexs[index].getCoordinates();
                        if(boundingBox.isInside(vIn)) {
                            if(vOut.y()>0) {
                                cg3::Segment2Dd topSide(cg3::Point2Dd(boundingBox.min().x(), boundingBox.max().y()),
                                                        boundingBox.max());
                                if (cg3::checkSegmentIntersection(cg3::Segment2Dd(vIn, vOut), topSide, true)) {
                                    double yBB = topSide.getP1().y();
                                    double xNew = vIn.x()+(yBB-vIn.y())*(vOut.x()-vIn.x())/(vOut.y()-vIn.y());
                                    edges[i].setOrigin(dcel.addVertex(Vertex(cg3::Point2Dd(xNew, yBB),i)));
                                    vertexs[index].setIncidEdge(std::numeric_limits<size_t>::max());
                                } else if(vOut.x() > 0) {
                                    cg3::Segment2Dd rightSide(cg3::Point2Dd(boundingBox.max().x(), boundingBox.min().y()),
                                                              boundingBox.max());
                                    if(cg3::checkSegmentIntersection(cg3::Segment2Dd(vIn, vOut), rightSide, true)) {
                                        double xBB = rightSide.getP1().x();
                                        double yNew = vIn.y()+(xBB-vIn.x())*(vOut.y()-vIn.y())/(vOut.x()-vIn.x());
                                        edges[i].setOrigin(dcel.addVertex(Vertex(cg3::Point2Dd(xBB, yNew),i)));
                                        vertexs[index].setIncidEdge(std::numeric_limits<size_t>::max());
                                    }
                                } else {
                                    cg3::Segment2Dd leftSide(cg3::Point2Dd(boundingBox.min().x(), boundingBox.max().y()),
                                                             boundingBox.min());
                                    if(cg3::checkSegmentIntersection(cg3::Segment2Dd(vIn, vOut), leftSide, true)) {
                                        double xBB = leftSide.getP1().x();
                                        double yNew = vIn.y()+(xBB-vIn.x())*(vOut.y()-vIn.y())/(vOut.x()-vIn.x());
                                        edges[i].setOrigin(dcel.addVertex(Vertex(cg3::Point2Dd(xBB, yNew),i)));
                                        vertexs[index].setIncidEdge(std::numeric_limits<size_t>::max());
                                    }
                                }
                            } else {
                                cg3::Segment2Dd bottomSide(cg3::Point2Dd(boundingBox.max().x(), boundingBox.min().y()),
                                                           boundingBox.min());
                                if(cg3::checkSegmentIntersection(cg3::Segment2Dd(vIn, vOut), bottomSide, true)) {
                                    double yBB = bottomSide.getP1().y();
                                    double xNew = vIn.x()+(yBB-vIn.y())*(vOut.x()-vIn.x())/(vOut.y()-vIn.y());
                                    edges[i].setOrigin(dcel.addVertex(Vertex(cg3::Point2Dd(xNew, yBB),i)));
                                    vertexs[index].setIncidEdge(std::numeric_limits<size_t>::max());
                                } else if(vOut.x() > 0) {
                                    cg3::Segment2Dd rightSide(cg3::Point2Dd(boundingBox.max().x(), boundingBox.min().y()),
                                                              boundingBox.max());
                                    if(cg3::checkSegmentIntersection(cg3::Segment2Dd(vIn, vOut), rightSide, true)) {
                                        double xBB = rightSide.getP1().x();
                                        double yNew = vIn.y()+(xBB-vIn.x())*(vOut.y()-vIn.y())/(vOut.x()-vIn.x());
                                        edges[i].setOrigin(dcel.addVertex(Vertex(cg3::Point2Dd(xBB, yNew),i)));
                                        vertexs[index].setIncidEdge(std::numeric_limits<size_t>::max());
                                    }
                                } else {
                                    cg3::Segment2Dd leftSide(cg3::Point2Dd(boundingBox.min().x(), boundingBox.max().y()),
                                                             boundingBox.min());
                                    if(cg3::checkSegmentIntersection(cg3::Segment2Dd(vIn, vOut), leftSide, true)) {
                                        double xBB = leftSide.getP1().x();
                                        double yNew = vIn.y()+(xBB-vIn.x())*(vOut.y()-vIn.y())/(vOut.x()-vIn.x());
                                        edges[i].setOrigin(dcel.addVertex(Vertex(cg3::Point2Dd(xBB, yNew),i)));
                                        vertexs[index].setIncidEdge(std::numeric_limits<size_t>::max());
                                    }
                                }
                            }
                        } else {
                            vertexs[edges[i].getOriginID()].setIncidEdge(std::numeric_limits<size_t>::max());
                            edges[i].setOrigin(std::numeric_limits<size_t>::max());
                        }
                    } else {
                        vertexs[edges[i].getOriginID()].setIncidEdge(std::numeric_limits<size_t>::max());
                        edges[i].setOrigin(std::numeric_limits<size_t>::max());
                    }
                }

            }
        }
    }

}
