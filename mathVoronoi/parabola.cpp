#include "parabola.h"

namespace Voronoi {

    /**
     * @brief findIntersectionPoint: find the point/s where the parabolas, with p1 and p2 as focuses and c as same directrix, intersect
     * @param p1 focus of first parabola
     * @param p2 focuse of second parabola
     * @param c directrix of the 2 parabolas
     * @return a vector with one or two points of intersection, sorted by x-coordinate
     */
    std::vector<cg3::Point2Dd> findIntersectionPoint(const cg3::Point2Dd& p1, const cg3::Point2Dd& p2, const double& d) {
        std::vector<cg3::Point2Dd> result;
        //if focuses have same x-coordinate
        if(fabs(p1.x() - p2.x()) < POINT_EPSILON) {
            double y = (p1.y() + p2.y())/2;
            double x1 = p1.x() - sqrt(d*d - d*(p1.y()+p2.y()) + p1.y()*p2.y());
            double x2 = p1.x() + sqrt(d*d - d*(p1.y()+p2.y()) + p1.y()*p2.y());

            result.push_back(cg3::Point2Dd(x1, y));
            result.push_back(cg3::Point2Dd(x2, y));
        //if focuses have same y-coordinate
        } else if(fabs(p1.y() - p2.y()) < POINT_EPSILON) {
            double x = (p1.x() + p2.x())/2;
            double y = ((x - p1.x())*(x - p1.x())/(p1.y() - d) + p1.y() + d)/2;
            result.push_back(cg3::Point2Dd(x,y));
        //otherwise
        } else {
            double a = 0.5/(p1.y()-d), e = 0.5/(p2.y()-d);
            double B = a*p1.x() - e*p2.x();
            double c = a*pow(p1.x(),2.0) - 0.25/a + p1.y(), f = e*pow(p2.x(),2.0) - 0.25/e + p2.y();
            double D = sqrt(B*B - (a-e)*(c-f));

            double x1 = (B+D)/(a-e), x2 = (B-D)/(a-e);
            if (x1 < x2) {
                result.push_back(cg3::Point2Dd(x1, a*pow(x1,2.0) - 2*a*p1.x()*x1 + c));
                result.push_back(cg3::Point2Dd(x2, e*pow(x2,2.0) - 2*e*p2.x()*x2 + f));
            } else {
                result.push_back(cg3::Point2Dd(x2, e*pow(x2,2.0) - 2*e*p2.x()*x2 + f));
                result.push_back(cg3::Point2Dd(x1, a*pow(x1,2.0) - 2*a*p1.x()*x1 + c));
            }
        }
        return result;
    }
}
