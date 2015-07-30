#include "Mesh.h"

#include <QPoint>
#include <QVector3D>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#define LDI_RESOL 12

using namespace std;

/**
*	Constructor, loading the specified aiMesh
**/
Mesh::MeshEntry::MeshEntry(aiMesh *mesh, int idObject) {
    vbo[VERTEX_BUFFER] = 0;
    vbo[TEXCOORD_BUFFER] = 0;
    vbo[NORMAL_BUFFER] = 0;
    vbo[COLOR_BUFFER] = 0;
    vbo[INDEX_BUFFER] = 0;

    m_vertices = NULL;
    m_texCoords = NULL;
    m_normals = NULL;
    m_colors = NULL;
    m_indices = NULL;

    m_idObject = idObject;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    verticeCount = mesh->mNumVertices;
    faceCount = mesh->mNumFaces;
    elementCount = mesh->mNumFaces * 3;

    if (mesh->HasPositions()) {
        m_vertices = new float[mesh->mNumVertices * 3];
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            m_vertices[i * 3] = mesh->mVertices[i].x;
            m_vertices[i * 3 + 1] = mesh->mVertices[i].y;
            m_vertices[i * 3 + 2] = mesh->mVertices[i].z;
        }

        glGenBuffers(1, &vbo[VERTEX_BUFFER]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
        glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), m_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray (0);
    }


    if (mesh->HasTextureCoords(0)) {
        m_texCoords = new float[mesh->mNumVertices * 2];
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            m_texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
            m_texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
        }

        glGenBuffers(1, &vbo[TEXCOORD_BUFFER]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);
        glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), m_texCoords, GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray (1);
    }


//    if (mesh->HasNormals()) {
//        float *normals = new float[mesh->mNumVertices * 3];
//        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
//            normals[i * 3] = mesh->mNormals[i].x;
//            normals[i * 3 + 1] = mesh->mNormals[i].y;
//            normals[i * 3 + 2] = mesh->mNormals[i].z;
//        }

//        glGenBuffers(1, &vbo[NORMAL_BUFFER]);
//        glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
//        glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);

//        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//        glEnableVertexAttribArray (2);

//        delete normals;
//    }


    m_colors = new float[mesh->mNumVertices * 3];
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        m_colors[i * 3] = 0.8f;
        m_colors[i * 3 + 1] = 0.8f;
        m_colors[i * 3 + 2] = 0.8f;
    }

    glGenBuffers(1, &vbo[COLOR_BUFFER]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[COLOR_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), m_colors, GL_STATIC_DRAW);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray (3);


    if (mesh->HasFaces()) {
        m_indices = new unsigned int[mesh->mNumFaces * 3];
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            m_indices[i * 3] = mesh->mFaces[i].mIndices[0];
            m_indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
            m_indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
        }

        glGenBuffers(1, &vbo[INDEX_BUFFER]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), m_indices, GL_STATIC_DRAW);

        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray (4);
    }



    // Generation des normales par faces
    m_normals = new float[mesh->mNumFaces * 3];
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        int indice_p1 = m_indices[i * 3];
        int indice_p2 = m_indices[i * 3 + 1];
        int indice_p3 = m_indices[i * 3 + 2];
        // Arete 1
        QVector3D v1;
        v1.setX(m_vertices[indice_p2 * 3] - m_vertices[indice_p1 * 3]);
        v1.setY(m_vertices[indice_p2 * 3 + 1] - m_vertices[indice_p1 * 3 + 1]);
        v1.setZ(m_vertices[indice_p2 * 3 + 2] - m_vertices[indice_p1 * 3 + 2]);
        // Arete 2
        QVector3D v2;
        v2.setX(m_vertices[indice_p3 * 3] - m_vertices[indice_p2 * 3]);
        v2.setY(m_vertices[indice_p3 * 3 + 1] - m_vertices[indice_p2 * 3 + 1]);
        v2.setZ(m_vertices[indice_p3 * 3 + 2] - m_vertices[indice_p2 * 3 + 2]);
        // Normal
        QVector3D n = QVector3D::normal(v1, v2);
        m_normals[i * 3] = n.x();
        m_normals[i * 3 + 1] = n.y();
        m_normals[i * 3 + 2] = n.z();
    }
}

/**
*	Deletes the allocated OpenGL buffers
**/
Mesh::MeshEntry::~MeshEntry() {
    if(vbo[VERTEX_BUFFER]) {
        glDeleteBuffers(1, &vbo[VERTEX_BUFFER]);
        delete m_vertices;
    }

    if(vbo[TEXCOORD_BUFFER]) {
        glDeleteBuffers(1, &vbo[TEXCOORD_BUFFER]);
        delete m_texCoords;
    }

    if(vbo[NORMAL_BUFFER]) {
        glDeleteBuffers(1, &vbo[NORMAL_BUFFER]);
        delete m_normals;
    }

    if(vbo[INDEX_BUFFER]) {
        glDeleteBuffers(1, &vbo[INDEX_BUFFER]);
        delete m_indices;
    }

    glDeleteVertexArrays(1, &vao);
}

/**
*	Renders this MeshEntry
**/
void Mesh::MeshEntry::render() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}



// Echange les points p1 et p2
void inverserPoint2D(QPoint &p1, QPoint &p2) {
    QPoint tmp = p1;
    p1 = p2;
    p2 = tmp;
}

// Trie les points de haut en bas, puis de gauche a droite
void sortPoint2D(QPoint &p1, QPoint &p2, QPoint &p3) {
    // Trie les points de haut en bas
    if (p2.y() < p1.y()) {
        inverserPoint2D(p1, p2);
    }
    if (p3.y() < p2.y()) {
        inverserPoint2D(p2, p3);
    }
    if (p2.y() < p1.y()) {
        inverserPoint2D(p1, p2);
    }
    // Trie les points de gauche a droite
    if (p1.y() == p2.y() && p2.x() < p1.x()) {
        inverserPoint2D(p1, p2);
    }
    if (p2.y() == p3.y() && p3.x() < p2.x()) {
        inverserPoint2D(p2, p3);
    }
    if (p1.y() == p2.y() && p2.x() < p1.x()) {
        inverserPoint2D(p1, p2);
    }
}

// teste l'orientation du point C par rapport a la droite AB
// Vrai si le point C est a droite de AB
bool oriented2d(const QPoint &a, const QPoint &b, const QPoint &c) {
    return ( ((b.x()-a.x())*(c.y()-a.y()) - (b.y()-a.y())*(c.x()-a.x())) >= 0 );
}

// Retourne l'aire du triangle
float areaTriangle(QPoint &p1, QPoint p2, QPoint p3) {
    return 0.5 * abs((p2.x()-p1.x())*(p3.y()-p1.y()) - (p3.x()-p1.x())*(p2.y()-p1.y()));
}

// Calcul le bord gauche strict (pixel d'intersection le plus au bord sur chaque ligne)
vector<QPoint> * leftEdgeScan(const QPoint &p1, const QPoint &p2, bool left_or_top_edge) {
    vector<QPoint> * res = new vector<QPoint>(0);

    int point[2];
    point[0] = p1.x();
    point[1] = p1.y();

    int dx = p2.x() - p1.x();
    int dy = p2.y() - p1.y();
    int x_inc = (dx < 0 ? -1 : 1);
    int y_inc = (dy < 0 ? -1 : 1);
    int abs_dx = abs(dx);
    int abs_dy = abs(dy);
    bool c = (dx * dy > 0);

    if (dx == 0 && dy == 0) {
        // Remplissage d'un point
        res->push_back(QPoint(point[0], point[1]));
        return res;
    }
    if (dx == 0) {
        // Remplissage d'une ligne verticale
        int offset = (left_or_top_edge ? 0 : -1);
        for (int i = 0; i <= abs_dy; ++i) {
            res->push_back(QPoint(point[0]+offset, point[1]));
            point[1] += y_inc;
        }
        return res;
    } else if (dy == 0)  {
        // Remplissage d'une ligne horizontale
        int offset = (left_or_top_edge ? 0 : -1);
        for (int i = 0; i <= abs_dx; ++i) {
            res->push_back(QPoint(point[0], point[1]+offset));
            point[0] += x_inc;
        }
        return res;
    }

    if (abs_dx > abs_dy) {  // ok !
        int numerator = abs_dy;
        int denominator = abs_dx;
        int increment = denominator - numerator;
        if (c) {
            // Iteration sur x
            for (int i = 0; i <= abs_dx; ++i) {
                increment += numerator;
                if ((increment > denominator) || (increment == denominator && left_or_top_edge)) {
                    res->push_back(QPoint(point[0], point[1]));
                    point[0] += x_inc;
                    point[1] += y_inc;
                    increment -= denominator;
                } else {
                    point[0] += x_inc;
                }
            }
            if (!left_or_top_edge) {
                res->push_back(QPoint(point[0], point[1]));
            }

        } else {
            // Iteration sur x
            point[0] -= x_inc;
            for (int i = 0; i <= abs_dx; ++i) {
              increment += numerator;
              if ((increment > denominator) || (increment == denominator && !left_or_top_edge)) {
                  res->push_back(QPoint(point[0], point[1]));
                  point[0] += x_inc;
                  point[1] += y_inc;
                  increment -= denominator;
              } else {
                  point[0] += x_inc;
              }
            }
            if (left_or_top_edge) {
                res->push_back(QPoint(point[0], point[1]));
            }
        }

    } else if (abs_dx < abs_dy) {   // ok !
        int numerator = abs_dx;
        int denominator = abs_dy;
        // Iteration sur y
        int increment = denominator - numerator;
        if (c) {
            for (int i = 0; i <= abs_dy; ++i) {
                increment += numerator;
                if ((increment > denominator) || (increment == denominator && !left_or_top_edge)) {
                    point[0] += x_inc;
                    increment -= denominator;
                }
                res->push_back(QPoint(point[0], point[1]));
                point[1] += y_inc;
            }
        } else {
            point[0] -= x_inc;
            for (int i = 0; i <= abs_dy; ++i) {
                increment += numerator;
                if ((increment > denominator) || (increment == denominator && left_or_top_edge)) {
                    point[0] += x_inc;
                    increment -= denominator;
                }
                res->push_back(QPoint(point[0], point[1]));
                point[1] += y_inc;
            }
        }

    } else {    // ok !
        // Pour les diagonales pures et parfaites
        if (!left_or_top_edge) {
            point[0] -= (y_inc == 1 ? -1 : 1);
        }
        for (int i = 0; i <= abs_dx; ++i) {
            res->push_back(QPoint(point[0], point[1]));
            point[0] += x_inc;
            point[1] += y_inc;
        }

    }

    return res;
}

// Voxelise le triangle projete sur xOy
vector<QPoint> * getPointFromTriangle(const QPoint &_p1, const QPoint &_p2, const QPoint &_p3) {
    vector<QPoint> * res = new vector<QPoint>();

    QPoint p1 = QPoint(_p1.x(), _p1.y());
    QPoint p2 = QPoint(_p2.x(), _p2.y());
    QPoint p3 = QPoint(_p3.x(), _p3.y());

    if (p1.x() == p2.x() && p2.x() == p3.x() && p1.y() == p2.y() && p2.y() == p3.y()) {
        return res;
    }
    if (p1.x() == p2.x() && p2.x() == p3.x()) {
        return res;
    }
    if (p1.y() == p2.y() && p2.y() == p3.y()) {
        return res;
    }

    vector<QPoint> * lineP1P2 = NULL;
    vector<QPoint> * lineP2P3 = NULL;
    vector<QPoint> * lineP3P1 = NULL;

    sortPoint2D(p1, p2, p3);
    if (oriented2d(p1, p2, p3)) {
        lineP1P2 = leftEdgeScan(p1, p3, true);
        lineP2P3 = leftEdgeScan(p3, p2, false);
        lineP3P1 = leftEdgeScan(p2, p1, (p2.y() - p1.y() == 0));
    } else {
        lineP1P2 = leftEdgeScan(p1, p2, true);
        lineP2P3 = leftEdgeScan(p2, p3, (p3.y() - p2.y() > 0));
        lineP3P1 = leftEdgeScan(p3, p1, false);
    }

    int indice1, indice2, indice3;
    QPoint tmpP1, tmpP2;

    // Bottom Flat Triangle
    if (p3.y() == p2.y()) {
        indice1 = 0;
        indice3 = lineP3P1->size()-1;
        while (indice1 < (int)lineP1P2->size()-1 && indice3 >= 0) {
            tmpP1 = (*lineP1P2)[indice1];
            tmpP2 = (*lineP3P1)[indice3];
            for (int x = tmpP1.x(); x <= tmpP2.x(); ++x) {
                res->push_back(QPoint(x, tmpP1.y()));
            }
            indice1++;
            indice3--;
        }

    // Top Flat Triangle
    } else if (p2.y() == p1.y()) {
        indice1 = 0;
        indice2 = lineP2P3->size()-1;
        while (indice1 < (int)lineP1P2->size() && indice2 >= 0) {
            tmpP1 = (*lineP1P2)[indice1];
            tmpP2 = (*lineP2P3)[indice2];
            for (int x = tmpP1.x(); x <= tmpP2.x(); ++x) {
                res->push_back(QPoint(x, tmpP1.y()));
            }
            indice1++;
            indice2--;
        }

    } else {
        indice1 = 0;
        indice3 = lineP3P1->size()-1;
        // Remplissage du triangle
        while (indice1 < (int)lineP1P2->size() && indice3 >= 0) {
            tmpP1 = (*lineP1P2)[indice1];
            tmpP2 = (*lineP3P1)[indice3];
            for (int x = tmpP1.x(); x <= tmpP2.x(); ++x) {
                res->push_back(QPoint(x, tmpP1.y()));
            }
            indice1++;
            indice3--;
        }

        if (indice3 == -1) {
            indice2 = lineP2P3->size()-2;
            while (indice1 < (int)lineP1P2->size() && indice2 >= 0) {
                tmpP1 = (*lineP1P2)[indice1];
                tmpP2 = (*lineP2P3)[indice2];
                for (int x = tmpP1.x(); x <= tmpP2.x(); ++x) {
                    res->push_back(QPoint(x, tmpP1.y()));
                }
                indice1++;
                indice2--;
            }

        } else {
            indice2 = 1;
            while (indice2 < (int)lineP2P3->size() && indice3 >= 0) {
                tmpP1 = (*lineP2P3)[indice2];
                tmpP2 = (*lineP3P1)[indice3];
                for (int x = tmpP1.x(); x <= tmpP2.x(); ++x) {
                    res->push_back(QPoint(x, tmpP1.y()));
                }
                indice2++;
                indice3--;
            }
        }
    }

    delete lineP1P2;
    delete lineP2P3;
    delete lineP3P1;

    return res;
}


void Mesh::MeshEntry::rasterisationX(vector<Fragment> &xLDI)
{
    int i, j, indice;
    float area1, area2, area3, d, x, nx;
    QVector3D p1, p2, p3;
    QPoint proj_p1, proj_p2, proj_p3;
    for (unsigned int k = 0; k < faceCount; ++k) {
        // Premier point
        indice = m_indices[k * 3];
        p1.setX(m_vertices[indice * 3]);
        p1.setY(m_vertices[indice * 3 + 1]);
        p1.setZ(m_vertices[indice * 3 + 2]);
        // Premiere normale
        nx = m_normals[k * 3];
        // Deuxieme point
        indice = m_indices[k * 3 + 1];
        p2.setX(m_vertices[indice * 3]);
        p2.setY(m_vertices[indice * 3 + 1]);
        p2.setZ(m_vertices[indice * 3 + 2]);
        // Troisieme point
        indice = m_indices[k * 3 + 2];
        p3.setX(m_vertices[indice * 3]);
        p3.setY(m_vertices[indice * 3 + 1]);
        p3.setZ(m_vertices[indice * 3 + 2]);

        proj_p1.setX(p1.y() * LDI_RESOL);
        proj_p1.setY(p1.z() * LDI_RESOL);
        proj_p2.setX(p2.y() * LDI_RESOL);
        proj_p2.setY(p2.z() * LDI_RESOL);
        proj_p3.setX(p3.y() * LDI_RESOL);
        proj_p3.setY(p3.z() * LDI_RESOL);

        vector<QPoint> * triangle = getPointFromTriangle(proj_p1, proj_p2, proj_p3);
        for (QPoint p : *triangle) {
            area1 = areaTriangle(proj_p2, proj_p3, p);
            area2 = areaTriangle(proj_p3, proj_p1, p);
            area3 = areaTriangle(proj_p1, proj_p2, p);
            d = area1 + area2 + area3;
            x = ((p1.x()*area1 + p2.x()*area2 + p3.x()*area3) / d);
            i = p.x();
            j = p.y();
            Fragment frag;
            // Attributs du fragment
            frag.setObject(m_idObject);
            frag.setTriangle(k);
            frag.setIJ(i, j);
            frag.setDepth(x);
            frag.setBarycentriqueCoord(area1 / d, area2 / d);
            frag.setIn(nx > 0);
            xLDI.push_back(frag);
        }
        delete triangle;
        triangle = NULL;
    }
}

void Mesh::MeshEntry::rasterisationY(vector<Fragment> &yLDI)
{
    int i, j, indice;
    float area1, area2, area3, d, y, ny;
    QVector3D p1, p2, p3;
    QPoint proj_p1, proj_p2, proj_p3;
    for (unsigned int k = 0; k < faceCount; ++k) {
        // Premier point
        indice = m_indices[k * 3];
        p1.setX(m_vertices[indice * 3]);
        p1.setY(m_vertices[indice * 3 + 1]);
        p1.setZ(m_vertices[indice * 3 + 2]);
        // Premiere normale
        ny = m_normals[k * 3 + 1];
        // Deuxieme point
        indice = m_indices[k * 3 + 1];
        p2.setX(m_vertices[indice * 3]);
        p2.setY(m_vertices[indice * 3 + 1]);
        p2.setZ(m_vertices[indice * 3 + 2]);
        // Troisieme point
        indice = m_indices[k * 3 + 2];
        p3.setX(m_vertices[indice * 3]);
        p3.setY(m_vertices[indice * 3 + 1]);
        p3.setZ(m_vertices[indice * 3 + 2]);

        proj_p1.setX(p1.x() * LDI_RESOL);
        proj_p1.setY(p1.z() * LDI_RESOL);
        proj_p2.setX(p2.x() * LDI_RESOL);
        proj_p2.setY(p2.z() * LDI_RESOL);
        proj_p3.setX(p3.x() * LDI_RESOL);
        proj_p3.setY(p3.z() * LDI_RESOL);

        vector<QPoint> * triangle = getPointFromTriangle(proj_p1, proj_p2, proj_p3);
        for (QPoint p : *triangle) {
            area1 = areaTriangle(proj_p2, proj_p3, p);
            area2 = areaTriangle(proj_p3, proj_p1, p);
            area3 = areaTriangle(proj_p1, proj_p2, p);
            d = area1 + area2 + area3;
            y = ((p1.y()*area1 + p2.y()*area2 + p3.y()*area3) / d);
            i = p.x();
            j = p.y();
            Fragment frag;
            // Attributs du fragment
            frag.setObject(m_idObject);
            frag.setTriangle(k);
            frag.setIJ(i, j);
            frag.setDepth(y);
            frag.setBarycentriqueCoord(area1 / d, area2 / d);
            frag.setIn(ny > 0);
            yLDI.push_back(frag);
        }
        delete triangle;
        triangle = NULL;
    }
}

void Mesh::MeshEntry::rasterisationZ(vector<Fragment> &zLDI)
{
    int i, j, indice;
    float area1, area2, area3, d, z, nz;
    QVector3D p1, p2, p3;
    QPoint proj_p1, proj_p2, proj_p3;
    for (unsigned int k = 0; k < faceCount; ++k) {
        // Premier point
        indice = m_indices[k * 3];
        p1.setX(m_vertices[indice * 3]);
        p1.setY(m_vertices[indice * 3 + 1]);
        p1.setZ(m_vertices[indice * 3 + 2]);
        nz = m_normals[k * 3 + 2];
        // Deuxieme point
        indice = m_indices[k * 3 + 1];
        p2.setX(m_vertices[indice * 3]);
        p2.setY(m_vertices[indice * 3 + 1]);
        p2.setZ(m_vertices[indice * 3 + 2]);
        // Troisieme point
        indice = m_indices[k * 3 + 2];
        p3.setX(m_vertices[indice * 3]);
        p3.setY(m_vertices[indice * 3 + 1]);
        p3.setZ(m_vertices[indice * 3 + 2]);

        proj_p1.setX(p1.x() * LDI_RESOL);
        proj_p1.setY(p1.y() * LDI_RESOL);
        proj_p2.setX(p2.x() * LDI_RESOL);
        proj_p2.setY(p2.y() * LDI_RESOL);
        proj_p3.setX(p3.x() * LDI_RESOL);
        proj_p3.setY(p3.y() * LDI_RESOL);

        vector<QPoint> * triangle = getPointFromTriangle(proj_p1, proj_p2, proj_p3);
        for (QPoint p : *triangle) {
            area1 = areaTriangle(proj_p2, proj_p3, p);
            area2 = areaTriangle(proj_p3, proj_p1, p);
            area3 = areaTriangle(proj_p1, proj_p2, p);
            d = area1 + area2 + area3;
            z = ((p1.z()*area1 + p2.z()*area2 + p3.z()*area3) / d);
            i = p.x();
            j = p.y();
            Fragment frag;
            // Attributs du fragment
            frag.setObject(m_idObject);
            frag.setTriangle(k);
            frag.setIJ(i, j);
            frag.setDepth(z);
            frag.setBarycentriqueCoord(area1 / d, area2 / d);
            frag.setIn(nz > 0);
            zLDI.push_back(frag);
        }
        delete triangle;
        triangle = NULL;
    }
}




/**
*	Mesh constructor, loads the specified filename if supported by Assimp
**/
Mesh::Mesh(const char *filename, int idObject)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, aiProcess_JoinIdenticalVertices);
    if(!scene) {
        printf("Unable to load mesh: %s\n", importer.GetErrorString());
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        meshEntries.push_back(new Mesh::MeshEntry(scene->mMeshes[i], idObject));
    }
}

/**
*	Clears all loaded MeshEntries
**/
Mesh::~Mesh()
{
    for (unsigned int i = 0; i < meshEntries.size(); ++i) {
        delete meshEntries.at(i);
    }
    meshEntries.clear();
}

/**
*	Renders all loaded MeshEntries
**/
void Mesh::render() {
    for (unsigned int i = 0; i < meshEntries.size(); ++i) {
        meshEntries.at(i)->render();
    }
}

/**
 *  Computes the LDI in the X direction
 */
void Mesh::rasterisationX(vector<Fragment> &xLDI)
{
    for (unsigned int i = 0; i < meshEntries.size(); ++i) {
        meshEntries.at(i)->rasterisationX(xLDI);
    }
}

/**
 *  Computes the LDI in the Y direction
 */
void Mesh::rasterisationY(vector<Fragment> &yLDI)
{
    for (unsigned int i = 0; i < meshEntries.size(); ++i) {
        meshEntries.at(i)->rasterisationY(yLDI);
    }
}

/**
 *  Computes the LDI in the Z direction
 */
void Mesh::rasterisationZ(vector<Fragment> &zLDI)
{
    for (unsigned int i = 0; i < meshEntries.size(); ++i) {
        meshEntries.at(i)->rasterisationZ(zLDI);
    }
}
