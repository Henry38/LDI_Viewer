#include <QVector3D>

#include "Viewer.h"
#include "StandardCamera.h"

#define LDI_RESOL 12    // 12 fragments pour 1 unite
#define GRID_RESOL 4    // 4 cellules pour 1 unite

using namespace std;
using namespace qglviewer;

Viewer::Viewer(StandardCamera* nfc) : QGLViewer()
{
    // Change the camera.
    Camera* c = camera();
    setCamera(nfc);
    delete c;

    mMesh = vector<Mesh*>(2);
    for (unsigned int i = 0; i < mMesh.size(); ++i) {
        mMesh[i] = NULL;
    }

    xLDI = vector<Fragment>(0);
    yLDI = vector<Fragment>(0);
    zLDI = vector<Fragment>(0);

    xCount = vector<int>(0);
    yCount = vector<int>(0);
    zCount = vector<int>(0);

    xListContact = vector<Contact*>();
    yListContact = vector<Contact*>();
    zListContact = vector<Contact*>();

    gradient = vector<vector<float>>(mMesh.size());

    listCellule = vector<Cellule>();
}

Viewer::~Viewer() {
    for (Contact *c : xListContact) {
        delete c;
    }
    for (Contact *c : yListContact) {
        delete c;
    }
    for (Contact *c : zListContact) {
        delete c;
    }
    xListContact.clear();
    yListContact.clear();
    zListContact.clear();
    for (unsigned int i = 0; i < mMesh.size(); ++i) {
        delete mMesh.at(i);
    }
    mMesh.clear();
}



void drawPlane(const QVector3D &p1, const QVector3D &p2, const QVector3D &p3, const QVector3D &color) {
    glBegin(GL_TRIANGLES);
      glColor3f(color.x(), color.y(), color.z());
      glVertex3f(p1.x(), p1.y(), p1.z());
      glColor3f(color.x(), 0, color.z());
      glVertex3f(p2.x(), p2.y(), p2.z());
      glColor3f(color.x(), color.y(), color.z());
      glVertex3f(p3.x(), p3.y(), p3.z());
    glEnd();
}

void drawXRectangle(const QVector3D &p1, const QVector3D &p2, const QVector3D &color) {
    drawPlane(p1, p2, QVector3D((p1.x()+p2.x())/2, p1.y(), p2.z()), color);
    drawPlane(p1, p2, QVector3D((p1.x()+p2.x())/2, p2.y(), p1.z()), color);
}

void drawYRectangle(const QVector3D &p1, const QVector3D &p2, const QVector3D &color) {
    drawPlane(p1, p2, QVector3D(p1.x(), (p1.y()+p2.y())/2, p2.z()), color);
    drawPlane(p1, p2, QVector3D(p2.x(), (p1.y()+p2.y())/2, p1.z()), color);
}

void drawZRectangle(const QVector3D &p1, const QVector3D &p2, const QVector3D &color) {
    drawPlane(p1, p2, QVector3D(p1.x(), p2.y(), (p1.z()+p2.z())/2), color);
    drawPlane(p1, p2, QVector3D(p2.x(), p1.y(), (p1.z()+p2.z())/2), color);
}

void drawCellule(const Cellule &c) {
    float i = float(c.x()) / GRID_RESOL;
    float j = float(c.y()) / GRID_RESOL;
    float k = float(c.z()) / GRID_RESOL;
    float p = 1.0f / GRID_RESOL;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 0.0, 1.0, 0.25);
    glBegin(GL_LINE_STRIP);
        glVertex3f(i, j, k);
        glVertex3f(i+p, j, k);
        glVertex3f(i+p, j+p, k);
        glVertex3f(i, j+p, k);
        glVertex3f(i, j, k);
    glEnd();
    glBegin(GL_LINE_STRIP);
        glVertex3f(i, j, k+p);
        glVertex3f(i+p, j, k+p);
        glVertex3f(i+p, j+p, k+p);
        glVertex3f(i, j+p, k+p);
        glVertex3f(i, j, k+p);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(i, j, k);
        glVertex3f(i, j, k+p);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(i+p, j, k);
        glVertex3f(i+p, j, k+p);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(i+p, j+p, k);
        glVertex3f(i+p, j+p, k+p);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(i, j+p, k);
        glVertex3f(i, j+p, k+p);
    glEnd();
    glDisable(GL_BLEND);
}



/**
 *  Tri la LDI associee au compteur de fragment
**/
void sortLDI(vector<Fragment> &ldi, vector<vector<int>> &count) {
    int k = 0;
    for (size_t i = 0; i < count.size(); ++i) {
        for (size_t j = 0; j < count[i].size(); ++j) {
            if (count[i][j] > 0) {
                sort(ldi.begin()+k, ldi.begin()+k+count[i][j]);
                k += count[i][j];
            }
        }
    }
}

int indexOfContact(vector<Contact*> &listContact, int obj1, int obj2, Cellule::Coord &coord) {
    int index = -1;
    Contact *c = NULL;
    for (unsigned int i = 0; i < listContact.size() && index == -1; ++i) {
        c = listContact[i];
        if (c->equal(obj1, obj2, coord)) {
            index = i;
        }
    }
    return index;
}



/**
 *  Calcul des LDI dans les directions x, y, z
**/
void Viewer::ldiComputation()
{
    int i, j;
    unsigned int k;

    xLDI.clear();
    zLDI.clear();
    yLDI.clear();
    xCount.clear();
    yCount.clear();
    zCount.clear();

    // Remplissage des LDI
    for (unsigned int i = 0; i < mMesh.size(); ++i) {
        mMesh[i]->rasterisationX(xLDI);
        mMesh[i]->rasterisationY(yLDI);
        mMesh[i]->rasterisationZ(zLDI);
    }

    // Tri des LDI
    sort(xLDI.begin(), xLDI.end());
    sort(yLDI.begin(), yLDI.end());
    sort(zLDI.begin(), zLDI.end());

    // Calcul des indices des debuts de liste
    k = 0;
    xCount.push_back(0);
    while (k < xLDI.size()) {
        i = xLDI[k].i();
        j = xLDI[k].j();
        do {
            k++;
        } while(xLDI[k].i() == i && xLDI[k].j() == j && k < xLDI.size());
        xCount.push_back(k);
    }

    k = 0;
    yCount.push_back(0);
    while (k < yLDI.size()) {
        i = yLDI[k].i();
        j = yLDI[k].j();
        do {
            k++;
        } while(yLDI[k].i() == i && yLDI[k].j() == j && k < yLDI.size());
        yCount.push_back(k);
    }

    k = 0;
    zCount.push_back(0);
    while (k < zLDI.size()) {
        i = zLDI[k].i();
        j = zLDI[k].j();
        do {
            k++;
        } while(zLDI[k].i() == i && zLDI[k].j() == j && k < zLDI.size());
        zCount.push_back(k);
    }
}

/**
 *  Calcul des contacts dans les directions x, y, z
 */
void Viewer::contactComputation()
{
    int acc, index;
    Fragment *frag = NULL;
    Fragment *topFrag = NULL;
    vector<float> listDepth = vector<float>();
    Cellule::Coord coord;

    float area = 1.0f / (LDI_RESOL * LDI_RESOL);

    // Calcul des contacts en x
    for (unsigned int indice = 1; indice < xCount.size(); ++indice) {
        acc = 0;
        for (int k = xCount[indice - 1]; k < xCount[indice]; ++k) {
            frag = &xLDI[k];

            if (frag->in()) {
                acc += 1;
                // Collision detectee
                if (acc == 2) {
                    topFrag = frag;
                }
            }

            if (!frag->in()) {
                // Fin de collision
                if (acc == 2) {
                    listDepth.clear();
                    listDepth.push_back(frag->depth() * GRID_RESOL);
                    for (int depth = floor(frag->depth() * GRID_RESOL)+1; depth <= floor(topFrag->depth() * GRID_RESOL); ++depth) {
                        listDepth.push_back(depth);
                    }
                    listDepth.push_back(topFrag->depth() * GRID_RESOL);

                    coord.y = floor( (float(frag->i()) / LDI_RESOL) * GRID_RESOL );
                    coord.z = floor( (float(frag->j()) / LDI_RESOL) * GRID_RESOL );

                    float bottomDepth, topDepth;
                    for (unsigned int j = 1; j < listDepth.size(); ++j) {
                        topDepth = listDepth[j];
                        bottomDepth = listDepth[j - 1];
                        coord.x = floor( bottomDepth );

                        index = indexOfContact(xListContact, frag->idObject(), topFrag->idObject(), coord);
                        if (index == -1) {
                            xListContact.push_back(new Contact(frag->idObject(), topFrag->idObject(), coord));
                            index = xListContact.size() - 1;
                        }

                        xListContact[index]->addVolume(area * (topDepth - bottomDepth) / GRID_RESOL);
                        accGradient(xListContact[index], frag, topFrag, bottomDepth, topDepth);
                    }
                }
                acc -= 1;
            }
        }
    }

    // Calcul des contacts en y
    for (unsigned int indice = 1; indice < yCount.size(); ++indice) {
        acc = 0;
        for (int k = yCount[indice - 1]; k < yCount[indice]; ++k) {
            frag = &yLDI[k];

            if (frag->in()) {
                acc += 1;
                // Collision detectee
                if (acc == 2) {
                    topFrag = frag;
                }
            }

            if (!frag->in()) {
                // Fin de collision
                if (acc == 2) {
                    listDepth.clear();
                    listDepth.push_back(frag->depth() * GRID_RESOL);
                    for (int depth = floor(frag->depth() * GRID_RESOL)+1; depth <= floor(topFrag->depth() * GRID_RESOL); ++depth) {
                        listDepth.push_back(depth);
                    }
                    listDepth.push_back(topFrag->depth() * GRID_RESOL);

                    coord.x = floor( (float(frag->i()) / LDI_RESOL) * GRID_RESOL );
                    coord.z = floor( (float(frag->j()) / LDI_RESOL) * GRID_RESOL );

                    float bottomDepth, topDepth;
                    for (unsigned int j = 1; j < listDepth.size(); ++j) {
                        topDepth = listDepth[j];
                        bottomDepth = listDepth[j - 1];
                        coord.y = floor( bottomDepth );

                        index = indexOfContact(yListContact, frag->idObject(), topFrag->idObject(), coord);
                        if (index == -1) {
                            yListContact.push_back(new Contact(frag->idObject(), topFrag->idObject(), coord));
                            index = yListContact.size() - 1;
                        }

                        yListContact[index]->addVolume(area * (topDepth - bottomDepth) / GRID_RESOL);
                        accGradient(yListContact[index], frag, topFrag, bottomDepth, topDepth);
                    }
                }
                acc -= 1;
            }
        }
    }

    // Calcul des contacts en z
    for (unsigned int indice = 1; indice < zCount.size(); ++indice) {
        acc = 0;
        for (int k = zCount[indice - 1]; k < zCount[indice]; ++k) {
            frag = &zLDI[k];

            if (frag->in()) {
                acc += 1;
                // Collision detectee
                if (acc == 2) {
                    topFrag = frag;
                }
            }

            if (!frag->in()) {
                // Fin de collision
                if (acc == 2) {
                    listDepth.clear();
                    listDepth.push_back(frag->depth() * GRID_RESOL);
                    for (int depth = floor(frag->depth() * GRID_RESOL)+1; depth <= floor(topFrag->depth() * GRID_RESOL); ++depth) {
                        listDepth.push_back(depth);
                    }
                    listDepth.push_back(topFrag->depth() * GRID_RESOL);

                    coord.x = floor( (float(frag->i()) / LDI_RESOL) * GRID_RESOL );
                    coord.y = floor( (float(frag->j()) / LDI_RESOL) * GRID_RESOL );

                    float bottomDepth, topDepth;
                    for (unsigned int j = 1; j < listDepth.size(); ++j) {
                        topDepth = listDepth[j];
                        bottomDepth = listDepth[j - 1];
                        coord.z = floor( bottomDepth );

                        index = indexOfContact(zListContact, frag->idObject(), topFrag->idObject(), coord);
                        if (index == -1) {
                            zListContact.push_back(new Contact(frag->idObject(), topFrag->idObject(), coord));
                            index = zListContact.size() - 1;
                        }

                        zListContact[index]->addVolume(area * (topDepth - bottomDepth) / GRID_RESOL);
                        accGradient(zListContact[index], frag, topFrag, bottomDepth, topDepth);
                    }
                }
                acc -= 1;
            }
        }
    }

}

/**
 *  Accumulation du gradient dans le contact en fonction des deux fragments extremites
 */
void Viewer::accGradient(Contact *contact, Fragment *downFrag, Fragment *topFrag, float bottomDepth, float topDepth)
{
    int id, idTriangle;
    unsigned int indice_p1, indice_p2, indice_p3;
    float area = 1.0f / (LDI_RESOL * LDI_RESOL);

    // Coefficient d'interpolation
    float bottomW = (float(bottomDepth) - downFrag->depth()) / (topFrag->depth() - downFrag->depth());
    float topW = (float(topDepth) - downFrag->depth()) / (topFrag->depth() - downFrag->depth());

    id = downFrag->idObject();
    idTriangle = downFrag->idTriangle();
    indice_p1 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle];
    indice_p2 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle + 1];
    indice_p3 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle + 2];
    contact->addGradient(id, indice_p1, area * (1.0-bottomW) * downFrag->b1());
    contact->addGradient(id, indice_p2, area * (1.0-bottomW) * downFrag->b2());
    contact->addGradient(id, indice_p3, area * (1.0-bottomW) * downFrag->b3());
    id = topFrag->idObject();
    idTriangle = topFrag->idTriangle();
    indice_p1 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle];
    indice_p2 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle + 1];
    indice_p3 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle + 2];
    contact->addGradient(id, indice_p1, area * bottomW * topFrag->b1());
    contact->addGradient(id, indice_p2, area * bottomW * topFrag->b2());
    contact->addGradient(id, indice_p3, area * bottomW * topFrag->b3());

    id = downFrag->idObject();
    idTriangle = downFrag->idTriangle();
    indice_p1 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle];
    indice_p2 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle + 1];
    indice_p3 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle + 2];
    contact->addGradient(id, indice_p1, area * -(1.0-topW) * downFrag->b1());
    contact->addGradient(id, indice_p2, area * -(1.0-topW) * downFrag->b2());
    contact->addGradient(id, indice_p3, area * -(1.0-topW) * downFrag->b3());
    id = topFrag->idObject();
    idTriangle = topFrag->idTriangle();
    indice_p1 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle];
    indice_p2 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle + 1];
    indice_p3 = mMesh[id]->meshEntries[0]->m_indices[3 * idTriangle + 2];
    contact->addGradient(id, indice_p1, area * -topW * topFrag->b1());
    contact->addGradient(id, indice_p2, area * -topW * topFrag->b2());
    contact->addGradient(id, indice_p3, area * -topW * topFrag->b3());
}


void Viewer::keyPressEvent(QKeyEvent *ke)
{
    // Switch between dot, wireframe and full render mode
    if (ke->key() == Qt::Key_A) {
        modeRender = (modeRender + 1) % 3;
        if (modeRender == 0) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        } else if (modeRender == 1) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        } else if (modeRender == 2) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
        }
        update();
    }

    // Switch between volume intersection, gradient and ldi display
    if (ke->key() == Qt::Key_Q) {
        modeDisplay = (modeDisplay + 1) % 3;
        update();
    }

    // Switch direction
    if (ke->key() == Qt::Key_Space) {
        direction = (direction + 1) % 3;
        update();
    }

    // Close the frame
    if (ke->key() == Qt::Key_Escape) {
        close();
    }
}

void Viewer::draw()
{
    // Affichage de la geometrie
    for (unsigned int  i = 0; i < mMesh.size(); ++i) {
        if (mMesh[i] != NULL) {
            mMesh[i]->render();
        }
    }

    int acc = 0;
    int i, j;
    float depth, volume;
    Fragment frag;

    vector<Fragment> *ldi = NULL;
    vector<int> *count = NULL;
    vector<Contact*> *listContact = NULL;
    if (direction == 0) {
        ldi = &xLDI;
        count = &xCount;
        listContact = &xListContact;
    } else if (direction == 1) {
        ldi = &yLDI;
        count = &yCount;
        listContact = &yListContact;
    } else if (direction == 2) {
        ldi = &zLDI;
        count = &zCount;
        listContact = &zListContact;
    }

    // Dessine les volumes d'intersection
    for (Cellule cell : listCellule) {
        drawCellule(cell);
    }

    if (modeDisplay == 0) {
        // Affichage des volumes d'intersections
        for (unsigned int indice = 1; indice < count->size(); ++indice) {
            acc = 0;
            for (int k = count->at(indice - 1); k < count->at(indice); ++k) {
                frag = ldi->at(k);

                if (frag.in()) {
                    acc += 1;
                    if (acc == 2) {
                        // Debut de collision detectee
                        depth = frag.depth();
                    }
                }

                if (!frag.in()) {
                    acc -= 1;
                    if (acc == 1) {
                        // Fin de collision detectee
                        volume = frag.depth() - depth;
                        i = frag.i();
                        j = frag.j();
                        glBegin(GL_LINES);
                        glColor3f(1.0, 0.0, 0.0);
                        if (direction == 0) {
                            glVertex3f(depth, (i+.5)/LDI_RESOL, (j+.5)/LDI_RESOL);
                            glVertex3f(depth + volume, (i+.5)/LDI_RESOL, (j+.5)/LDI_RESOL);
                        } else if (direction == 1) {
                            glVertex3f((i+.5)/LDI_RESOL, depth, (j+.5)/LDI_RESOL);
                            glVertex3f((i+.5)/LDI_RESOL, depth + volume, (j+.5)/LDI_RESOL);
                        } else if (direction == 2) {
                            glVertex3f((i+.5)/LDI_RESOL, (j+.5)/LDI_RESOL, depth);
                            glVertex3f((i+.5)/LDI_RESOL, (j+.5)/LDI_RESOL, depth + volume);
                        }
                        glEnd();
                    }
                }
            }
        }

    } else if (modeDisplay == 1) {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);

        float x, y, z;
        QVector3D vect;
        // Affichage des gradients sur chaque point (mono-volume)
        for (unsigned int id = 0; id < mMesh.size(); ++id) {
            for (unsigned int k = 0; k < mMesh[id]->meshEntries[0]->verticeCount; ++k) {
                x = mMesh[id]->meshEntries[0]->m_vertices[3 * k];
                y = mMesh[id]->meshEntries[0]->m_vertices[3 * k + 1];
                z = mMesh[id]->meshEntries[0]->m_vertices[3 * k + 2];

                vect.setX(gradient[id][3 * k]);
                vect.setY(gradient[id][3 * k + 1]);
                vect.setZ(gradient[id][3 * k + 2]);

                vect.normalize();
                vect /= 5;

                glBegin(GL_LINES);
                glColor3f(id / (float)mMesh.size(), 0.0, 1.0 - (id / (float)mMesh.size()));
                glVertex3f(x, y, z);
                glColor3f(1.0, 1.0, 1.0);
                glVertex3f(x+vect.x(), y+vect.y(), z+vect.z());
                glEnd();
            }
        }

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    } else if (modeDisplay == 2) {
        // Affichage de la LDI
        for (unsigned int indice = 1; indice < count->size(); ++indice) {
            acc = count->at(indice) - count->at(indice - 1) - 1;
            frag = ldi->at(count->at(indice) - 1);

            // On affiche que le dernier fragment de la sous liste
            i = frag.i();
            j = frag.j();
            float k = frag.depth();
            if (direction == 0) {
                k = -2;//(minX / RESOLUTION) - 1;
                drawXRectangle(QVector3D(k, float(i)/LDI_RESOL, float(j)/LDI_RESOL),
                               QVector3D(k, (i+1.0)/LDI_RESOL, (j+1.0)/LDI_RESOL),
                               QVector3D(1.0-acc/6.0, 1.0, acc/6.0));
            } else if (direction == 1) {
                k = -2;//(minY / RESOLUTION) - 1;
                drawYRectangle(QVector3D(float(i)/LDI_RESOL, k, float(j)/LDI_RESOL),
                               QVector3D((i+1.0)/LDI_RESOL, k, (j+1.0)/LDI_RESOL),
                               QVector3D(1.0-acc/6.0, 1.0, acc/6.0));
            } else if (direction == 2) {
                k = -2;//(minZ / RESOLUTION) - 1;
                drawZRectangle(QVector3D(float(i)/LDI_RESOL, float(j)/LDI_RESOL, k),
                               QVector3D((i+1.0)/LDI_RESOL, (j+1.0)/LDI_RESOL, k),
                               QVector3D(1.0-acc/6.0, 1.0, acc/6.0));
            }
        }
    }
}

void Viewer::init() {

    // Restore previous viewer state.
    restoreStateFromFile();

    setAxisIsDrawn(true);

    if (glewInit() != GLEW_OK) {
        cerr << "Warning: glewInit failed!" << endl;
    }

    // Couleur de fond de la scene
    glClearColor(0.0, 1.0, 0.0, 0.0);

    string path = "C:/Users/henrylefevre/Documents/QtProject/LDI_Viewer/model/";
    string models[2];
    models[0] = "sphere.stl";
    models[1] = "cube2.obj";

    // Importation des mesh et creation des gradients de chaque mesh
    for (unsigned int id = 0; id < mMesh.size(); ++id) {
        mMesh[id] = new Mesh((path + models[id]).c_str(), id);
        gradient[id] = vector<float>(3 * mMesh[id]->meshEntries[0]->verticeCount);
    }

    // Caclul de la LDIa
    ldiComputation();

    // Calcul des contacts entre paires d'objets
    contactComputation();

    // Calcul du gradient sur chaque point du maillage
    for (Contact *c : xListContact) {
        for (unsigned int id = 0; id < mMesh.size(); ++id) {
            for (unsigned int k = 0; k < mMesh[id]->meshEntries[0]->verticeCount; ++k) {
                gradient[id][3 * k] += c->gradient(id, k);
            }
        }
    }

    for (Contact *c : yListContact) {
        for (unsigned int id = 0; id < mMesh.size(); ++id) {
            for (unsigned int k = 0; k < mMesh[id]->meshEntries[0]->verticeCount; ++k) {
                gradient[id][3 * k + 1] += c->gradient(id, k);
            }
        }
    }

    for (Contact *c : zListContact) {
        for (unsigned int id = 0; id < mMesh.size(); ++id) {
            for (unsigned int k = 0; k < mMesh[id]->meshEntries[0]->verticeCount; ++k) {
                gradient[id][3 * k + 2] += c->gradient(id, k);
            }
        }
    }

    modeRender = 0;
    modeDisplay = 0;
    direction = 2;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
