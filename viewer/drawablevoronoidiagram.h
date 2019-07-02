#ifndef DRAWABLEVORONOIDIAGRAM_H
#define DRAWABLEVORONOIDIAGRAM_H

#include <cg3/utilities/color.h>
#include "../data_structures/dcel.h"
#include <cg3/geometry/point.h>
#include <cg3/viewer/interfaces/drawable_object.h>
#include <math.h>

namespace Voronoi {

    class DrawableVoronoiDiagram : public DCEL, public cg3::DrawableObject {
        public:
            DrawableVoronoiDiagram();

            // DrawableObject interface
            void draw() const;
            cg3::Pointd sceneCenter() const;
            double sceneRadius() const;

            void setEdgeColor(const QColor& c);
            void setPointColor(const QColor& c);
            void setEdgeWidth(int w);
            void setPointSize(int s);

        private:

            void drawEdges() const;
            void drawPoints() const;

            int pointSize;
            cg3::Color pointColor;
            int edgeWidth;
            cg3::Color edgeColor;
    };

    inline void DrawableVoronoiDiagram::setEdgeColor(const QColor& c) {
        edgeColor = c;
    }

    inline void DrawableVoronoiDiagram::setPointColor(const QColor& c) {
        pointColor = c;
    }

    inline void DrawableVoronoiDiagram::setEdgeWidth(int w) {
        edgeWidth = w;
    }

    inline void DrawableVoronoiDiagram::setPointSize(int s) {
        pointSize = s;
    }
}

#endif // DRAWABLEVORONOIDIAGRAM_H
