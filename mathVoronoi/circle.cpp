#include "circle.h"

namespace Voronoi {

    bool findCircleCenter(const cg3::Point2Dd& a, const cg3::Point2Dd& b, const cg3::Point2Dd& c, cg3::Point2Dd& center) {
        double m1 = (a.x() - b.x())/(b.y() - a.y()),
                m2 = (a.x() - c.x())/(c.y() - a.y());

        if(abs(m1-m2) < 1.0e-6)
            return false;

        double x = 0.5 * (a.x() + (b.x()*m1 - b.y() + c.y() - c.x()*m2)/(m1 - m2));
        double y = 0.5 * (a.y() + b.y() + (2*x - a.x() - b.x())*m1);

        center.set(x,y);

        return true;
    }

}
