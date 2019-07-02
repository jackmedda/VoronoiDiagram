#ifndef VORONOIDIAGRAM_H
#define VORONOIDIAGRAM_H

#include <../data_structures/dcel.h>
#include <../data_structures/beachline.h>
#include <../mathVoronoi/circle.h>
#include <../data_structures/event.h>
#include <../viewer/drawablevoronoidiagram.h>
#include <cg3/viewer/drawable_objects/2d/drawable_bounding_box2d.h>
#include <cg3/geometry/2d/intersections2d.h>
#include <queue>

namespace Voronoi {
    void fortuneAlgorithm(const std::vector<cg3::Point2Dd>& points, DrawableVoronoiDiagram& dcel,
                          const cg3::DrawableBoundingBox2D& boundingBox);
    void checkCircleEvent(const Leaf* l1, Leaf* middleArc, const Leaf* l3, const double& sweepline,
                          std::priority_queue<Event*, std::vector<Event*>, EventComparator>& pq);
}

#endif // VORONOIDIAGRAM_H
