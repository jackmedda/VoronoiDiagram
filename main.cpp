#include <cg3/viewer/mainwindow.h>

#include "managers/voronoimanager.h"

#include "utils/fileutils.h"
#include "algorithms/voronoidiagram.h"

int main(int argc, char *argv[]) {
    std::vector<cg3::Point2Dd> points = FileUtils::getPointsFromFile("../inpu_files/100.txt");
    Voronoi::fortuneAlgorithm(points);
    /*CG3_SUPPRESS_WARNING(argc);
    CG3_SUPPRESS_WARNING(argv);

    QApplication app(argc, argv);

    //Main window, it contains QGLViewer canvas
    cg3::viewer::MainWindow gui;

    //Set MainWindow options
    gui.set2DMode(true); //2D mode
    gui.setSelectLeftButton(); //Enable picking

    //Voronoi manager initialization
    VoronoiManager vm(&gui);
    gui.addManager(&vm, "Voronoi Manager");

    //Show the GUI
    gui.updateGlCanvas();
    gui.show();

    return app.exec();*/
}
