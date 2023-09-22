#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include <GL/glu.h>

using namespace std;

struct Vector3D {
    float x, y, z;
};

struct Vertex {
    float x, y, z;
    float u, v;
    vector<int> adjacentFaces;
    Vector3D normal;
};

struct Edge {
    int v1, v2;
};

struct VertexIndex {
    int v, vn, vt;
};

// for a face, it has more than 3 vertexes and a normal
struct Face{
    vector<VertexIndex> facevertex;
    Vector3D normal;
};

//added for set 5
struct TexCoord {
    float u, v;
};

class Obj {
    private:
        vector<Vertex> mVertices;
        vector<Face> mFaces;
        vector<Edge> mEdges;
        vector<TexCoord> mTexCoords;
        GLuint mTextureId;

    public:
        void readFile(const char* filename);
        void drawModelPoints() const;
        void drawModelLines() const;
        void drawModelSolid() const;
        void drawModelSolidFaceNormal() const;
        //5.1、5.2
        void computeFaceNormals(Face& face);
        void computeVertexNormals(Vertex& vertex);
        void loadTexture(const char* filename);
        void setAllVertexUV();
        //5.3
        void deformObject(float a, float t);
};

#endif
