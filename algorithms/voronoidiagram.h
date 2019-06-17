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

    struct Point2DComparator {
        bool operator()(const cg3::Point2Dd& p1, const cg3::Point2Dd& p2) {
            return p1.y() > p2.y() || (p1.y() - p2.y() < POINT_EPSILON && p1.x() > p2.x());
        }
    };

    struct EventComparator {
        bool operator()(const Event* e1, const Event* e2) {
            return Point2DComparator().operator()(e1->point, e2->point);
        }
    };

    void fortuneAlgorithm(const std::vector<cg3::Point2Dd>& points);
    Event* checkCircleEvent(const Leaf* l1, Leaf* middleArc, const Leaf* l3, const double& sweepline);
}

#endif // VORONOIDIAGRAM_H
