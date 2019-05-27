#include "parabola.h"

namespace Voronoi {

    std::vector<cg3::Point2Dd> findIntersectionPoint(const cg3::Point2Dd& p1, const cg3::Point2Dd& p2, const double& c) {
        std::vector<cg3::Point2Dd> result;
        //if focuses have same x-coordinate
        if(fabs(p1.x() - p2.x()) < POINT_EPSILON) {
            double y = (p1.y() + p2.y())/2;
            double x1 = p1.x() + sqrt(c*c - c*(p1.y()+p2.y()) + p1.y()*p2.y());
            double x2 = p1.x() - sqrt(c*c - c*(p1.y()+p2.y()) + p1.y()*p2.y());
            if (x1 < x2) {
                result.push_back(cg3::Point2Dd(x1, y));
                result.push_back(cg3::Point2Dd(x2, y));
            } else {
                result.push_back(cg3::Point2Dd(x2, y));
                result.push_back(cg3::Point2Dd(x1, y));
            }
            return result;
        //if focuses have same y-coordinate
        } else if(fabs(p1.y() - p2.y()) < POINT_EPSILON) {
            double x = (p1.x() + p2.x())/2;
            double y = ((x - p1.x())*(x - p1.x())/(p1.y() - c) + p1.y() + c)/2;
            result.push_back(cg3::Point2Dd(x,y));
            return result;
        //otherwise
        } else {
            double a = 0.5*(p1.y()-c), e = 0.5*(p2.y()-c);
            double B = a*p1.x() - e*p2.x();
            double D = sqrt(B*B - (a-e)*(a*pow(p1.x(),2.0) + 0.25/a - e*pow(p2.x(),2.0) - 0.25/e));

            double x1 = (B+D)/(a-e), x2 = (B-D)/(a-e);
            if (x1 < x2) {
                result.push_back(cg3::Point2Dd(x1, a*pow(p1.x(),2.0) - 2*a*p1.x()*x1 + c + a*pow(p1.x(),2.0) + 0.25/a));
                result.push_back(cg3::Point2Dd(x2, e*pow(p2.x(),2.0) - 2*e*p2.x()*x2 + c + e*pow(p2.x(),2.0) + 0.25/e));
            } else {
                result.push_back(cg3::Point2Dd(x2, e*pow(p2.x(),2.0) - 2*e*p2.x()*x2 + c + e*pow(p2.x(),2.0) + 0.25/e));
                result.push_back(cg3::Point2Dd(x1, a*pow(p1.x(),2.0) - 2*a*p1.x()*x1 + c + a*pow(p1.x(),2.0) + 0.25/a));
            }
            return result;
        }
    }

}
