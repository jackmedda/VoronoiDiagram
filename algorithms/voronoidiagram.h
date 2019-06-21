#ifndef VORONOIDIAGRAM_H
#define VORONOIDIAGRAM_H

#include <vector>
#include <../data_structures/dcel.h>
#include <../data_structures/beachline.h>
#include <../mathVoronoi/circle.h>
#include <queue>

namespace Voronoi {

    struct Event {
        typedef enum {SITE, CIRCLE, FALSE} EventType;

        //Event attributes
        EventType type;
        const cg3::Point2Dd point;

        //Circle Events
        const cg3::Point2Dd center;
        const Voronoi::Leaf* arc;

        Event(EventType type, const cg3::Point2Dd& point) : type(type), point(point) {}
        Event(EventType type, const cg3::Point2Dd& point, const cg3::Point2Dd& center, const Voronoi::Leaf* arc):
            type(type), point(point), center(center), arc(arc) {}
    };

    struct EventComparator {
        bool operator()(const Event* e1, const Event* e2) {
            return e1->point.y() < e2->point.y();
        }
    };

    void fortuneAlgorithm(const std::vector<cg3::Point2Dd>& points);
    Event* checkCircleEvent(const Leaf* l1, Leaf* middleArc, const Leaf* l3, const double& sweepline);
}

#endif // VORONOIDIAGRAM_H
