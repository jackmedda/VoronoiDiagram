#ifndef CIRCLE_H
#define CIRCLE_H

#include <cg3/geometry/2d/point2d.h>

namespace Voronoi {

    bool findCircleCenter(const cg3::Point2Dd& a, const cg3::Point2Dd& b, const cg3::Point2Dd& c, cg3::Point2Dd& center);

}

#endif // CIRCLE_H
