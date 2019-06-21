#include "voronoidiagram.h"
#include <QDebug>

namespace Voronoi {

    Event* checkCircleEvent(const Leaf* l1, Leaf* middleArc, const Leaf* l3, const double& sweepline) {
        //if newPoint->next->next or newPoint->prev->prev is null
        if(!l3)
            return nullptr;

        cg3::Point2Dd center, circleEventPoint;

        if(!findCircleCenter(*(l1->site), *(middleArc->site), *(l3->site), center))
            return nullptr;

        circleEventPoint.set(center.x(), center.y() - center.dist(*(l1->site)));

        if(circleEventPoint.y() < sweepline) {
            middleArc->circleEvent = new Event(Event::EventType::CIRCLE, circleEventPoint, center, middleArc);
            return middleArc->circleEvent;
        }
        else
            return nullptr;
    }

    void fortuneAlgorithm(const std::vector<cg3::Point2Dd>& points) {
        DCEL dcel;
        double sweepline;
        Beachline beachline(&sweepline);
        std::priority_queue<Event*, std::vector<Event*>, EventComparator> pq;
        Node* r;

        for(const cg3::Point2Dd& p : points) {
            pq.push(new Event(Event::EventType::SITE, p));
        }

        while(!pq.empty()) {
            Event* e = pq.top();
            pq.pop();
            sweepline = e->point.y();

            qDebug() <<"Sweepline (new point y):"<< sweepline;
            r = beachline.getRoot();
            /*if(sweepline<413000.0) {
                r = beachline.getRoot();
            }
            if(!beachline.isBalanced(r)) {
                r = beachline.getRoot();
            }
            //qDebug() << "start beachline print";
            //beachline.postorder(beachline.getRoot());
            //beachline.printLevelOrder(beachline.getRoot());
            //qDebug() << "end beachline print";*/
            if(e->type == Event::EventType::SITE) {
                Leaf* newPoint = nullptr;
                Event* oldPointEvent = beachline.addPoint(e->point, newPoint, dcel.getHalfEdges());

                if(newPoint) {//if it is nullptr, the beachline was empty before the addPoint
                    if(oldPointEvent)
                        oldPointEvent->type = Event::EventType::FALSE;

                    Event* newEvent = checkCircleEvent(newPoint, newPoint->next, newPoint->next->next, sweepline);
                    if(newEvent)
                        pq.push(newEvent);

                    newEvent = checkCircleEvent(newPoint, newPoint->prev, newPoint->prev->prev, sweepline);
                    if(newEvent)
                        pq.push(newEvent);
                }
            }/* else if(e->type == Event::EventType::CIRCLE) {
                Leaf* prev = e->arc->prev;
                InternalNode* newBreakpoint;

                //Saving prev and next pointers of the arc that is going to be removed
                prev->next->next->prev = prev;
                prev->next = prev->next->next;

                newBreakpoint = beachline.removePoint(e->arc);

                //Create new vertex and connect halfEdges to it
                std::vector<HalfEdge>& edges = dcel.getHalfEdges();
                size_t lastIndex = edges.size();
                edges.push_back(Voronoi::HalfEdge());
                edges.back().setTwin(lastIndex+1);
                edges.push_back(Voronoi::HalfEdge());
                edges.back().setTwin(lastIndex);

                newBreakpoint->edge = lastIndex+1;
                dcel.addVertex(Vertex(e->center, newBreakpoint->edge));
                edges[static_cast<InternalNode*>(prev->parent)->edge].setOrigin(dcel.getVertexs().size()-1);
                edges[static_cast<InternalNode*>(prev->next->parent)->edge].setOrigin(dcel.getVertexs().size()-1);

                Event* newEvent = checkCircleEvent(prev->next, prev, prev->prev, sweepline);
                if(newEvent)
                    pq.push(newEvent);
                newEvent = checkCircleEvent(prev, prev->next, prev->next->next, sweepline);
            }*/

            //Free memory of the event
            delete e;
        }
    }

}
