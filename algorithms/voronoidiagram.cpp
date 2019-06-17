#include "voronoidiagram.h"

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

        for(const cg3::Point2Dd& p : points) {
            pq.push(new Event(Event::EventType::SITE, p));
        }

        while(!pq.empty()) {
            Event* e = pq.top();
            pq.pop();
            sweepline = e->point.y();

            if(e->type == Event::EventType::SITE) {
                Leaf* newPoint;
                Event* oldPointEvent = beachline.addPoint(e->point, newPoint, dcel.getHalfEdges());

                if(oldPointEvent)
                    oldPointEvent->type = Event::EventType::FALSE;

                Event* newEvent = checkCircleEvent(newPoint, newPoint->next, newPoint->next->next, sweepline);
                if(newEvent)
                    pq.push(newEvent);

                newEvent = checkCircleEvent(newPoint, newPoint->prev, newPoint->prev->prev, sweepline);
                if(newEvent)
                    pq.push(newEvent);
            } else if(e->type == Event::EventType::CIRCLE) {
                Leaf* prev = e->arc->prev;

                prev->next->next->prev = prev;
                prev->next = prev->next->next;

                beachline.removePoint(e->arc);
            }

            //Free memory of the event
            delete e;
        }
    }

}
