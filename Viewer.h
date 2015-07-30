#ifndef VIEWER_H
#define VIEWER_H

#include <QKeyEvent>

#include "Mesh.h"
#include "QGLViewer/qglviewer.h"
#include "Fragment.h"
#include "Contact.h"
#include "Cellule.h"

class StandardCamera;

class Viewer : public QGLViewer
{

    Q_OBJECT

public:
    Viewer(StandardCamera* camera);
    ~Viewer();

    void ldiComputation();
    void contactComputation();
    void accGradient(Contact *contact, Fragment *frag, Fragment *topFrag, float bottomDepth, float topDepth);

protected :
    virtual void keyPressEvent(QKeyEvent *ke);
    virtual void draw();
    virtual void init();

private:
    std::vector<Mesh*> mMesh;
    int modeRender, modeDisplay, direction;

    std::vector<Fragment> xLDI, yLDI, zLDI;
    std::vector<int> xCount, yCount, zCount;

    std::vector<Contact*> xListContact, yListContact, zListContact;

    std::vector<std::vector<float>> gradient;

    std::vector<Cellule> listCellule;
};

#endif // VIEWER_H
