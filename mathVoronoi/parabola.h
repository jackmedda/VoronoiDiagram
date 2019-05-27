#ifndef PARABOLA_H
#define PARABOLA_H

#include <cg3/geometry/2d/point2d.h>
#include <vector>

#define POINT_EPSILON 1.0e-6

namespace Voronoi {

    /**
     * @brief findIntersectionPoint
     * @param p1: point that represent the focus of the first parabola
     * @param p2: point that represent the focus of the second parabola
     * @param sweepline: it represent the directrix of the two parabolas
     * @return a vector containing the intersection points of the parabolas
     */
    std::vector<cg3::Point2Dd> findIntersectionPoint(const cg3::Point2Dd& p1, const cg3::Point2Dd& p2, const double& sweepline);

}

#endif // PARABOLA_H
