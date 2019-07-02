#include "event.h"

namespace Voronoi {
    Event::~Event() {}

    const cg3::Point2Dd& SiteEvent::getPoint() const { return point; }
    const cg3::Point2Dd& CircleEvent::getPoint() const { return point; }
}
