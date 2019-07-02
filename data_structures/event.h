#ifndef EVENT_H
#define EVENT_H

#include <cg3/geometry/2d/point2d.h>

namespace Voronoi {

    struct Leaf;
    class Event {

        public:
            typedef enum {SITE, CIRCLE, FALSE} EventType;

            virtual ~Event();

            EventType getType() const;

            void setType(const EventType eT);
            virtual const cg3::Point2Dd& getPoint() const = 0;

        protected:
            Event(const EventType type) : type(type) {}
        private :
            //Event attributes
            EventType type;
    };

    class SiteEvent : public Event {
        public:
            SiteEvent(const cg3::Point2Dd& point) : Event(EventType::SITE), point(point) {}

            const cg3::Point2Dd& getPoint() const;
        private:
            const cg3::Point2Dd& point;
    };

    class CircleEvent : public Event {
        public:
            CircleEvent (const cg3::Point2Dd point, const cg3::Point2Dd center, Leaf* arc):
                Event(EventType::CIRCLE), point(point), center(center), arc(arc) {}

            const cg3::Point2Dd& getPoint() const;
            const cg3::Point2Dd& getCircleCenter() const;
            const Leaf* getArc() const;
        private:
            const cg3::Point2Dd point;
            const cg3::Point2Dd center;
            Leaf* arc;
    };

    inline Event::EventType Event::getType() const { return type; }
    inline const cg3::Point2Dd& CircleEvent::getCircleCenter() const { return center; }
    inline const Leaf* CircleEvent::getArc() const { return arc; }

    inline void Event::setType(const EventType eT) { type = eT; }

    struct EventComparator {
        bool operator()(const Event* e1, const Event* e2) {
            return e1->getPoint().y() < e2->getPoint().y();
        }
    };

}

#endif // EVENT_H
