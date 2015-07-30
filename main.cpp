#include <QApplication>

#include "Viewer.h"
#include "StandardCamera.h"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    StandardCamera* sc = new StandardCamera();
    Viewer viewer(sc);

    viewer.show();

    return app.exec();
}
