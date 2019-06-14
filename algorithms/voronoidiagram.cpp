#include "voronoidiagram.h"

namespace Voronoi {

    void fortuneAlgorithm(const std::vector<cg3::Point2Dd>& points) {

        std::priority_queue<Event*, std::vector<Event*>, EventComparator> pq;

        for(const cg3::Point2Dd& p : points) {
            pq.push(new Event(Event::EventType::SITE, p));
        }

        while(!pq.empty()) {
            Event* e = pq.top();
            pq.pop();

            if(e->type == Event::EventType::SITE) {

            } else if(e->type == Event::EventType::CIRCLE) {

            }

            //Free memory of the event
            delete e;
        }
    }

}
