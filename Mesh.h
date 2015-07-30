#ifndef MESH_H
#define MESH_H

#include "GL/glew.h"

#include "assimp/scene.h"
#include "assimp/mesh.h"

#include "Fragment.h"

#include <QVector3D>
#include <vector>

class Mesh
{
public:
    struct MeshEntry {
        enum BUFFERS {
            VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER, COLOR_BUFFER, INDEX_BUFFER
        };
        GLuint vao;
        GLuint vbo[5];

        float *m_vertices;
        float *m_texCoords;
        float *m_normals;
        float *m_colors;
        unsigned int *m_indices;

        unsigned int verticeCount;
        unsigned int faceCount;
        unsigned int elementCount;
        int m_idObject;

        MeshEntry(aiMesh *mesh, int idObject);
        ~MeshEntry();

        float *vertices() { return m_vertices; }
        float *texCoords() { return m_texCoords; }
        float *colors() { return m_colors; }
        float *normals() { return m_normals; }
        unsigned int *indices() { return m_indices; }
        unsigned int idObject() { return m_idObject; }

        void load(aiMesh *mesh);
        void render();

        void rasterisationX(std::vector<Fragment> &xLDI);
        void rasterisationY(std::vector<Fragment> &yLDI);
        void rasterisationZ(std::vector<Fragment> &zLDI);
    };

    std::vector<MeshEntry*> meshEntries;

public:
    Mesh(const char *filename, int idObject);
    ~Mesh();

    void render();

    void rasterisationX(std::vector<Fragment> &xLDI);
    void rasterisationY(std::vector<Fragment> &yLDI);
    void rasterisationZ(std::vector<Fragment> &zLDI);

};

#endif // MESH_H
