#include "Obj.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

#include "stb_image.h"

using namespace std;

void Obj::readFile(const char* filename) {

    mVertices.clear();
    mFaces.clear();
    mEdges.clear();

    bool TextureMod = false;

    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (line.substr(0, 2) == "v ") {
                istringstream iss(line.substr(2));
                Vertex vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                mVertices.push_back(vertex);
            }
            // for texture
            else if (line.substr(0, 3) == "vt ") {
                istringstream iss(line.substr(3));
                float u, v;
                iss >> u >> v;
                mTexCoords.push_back({ u, v });
            }
            else if (line.substr(0, 2) == "f ") {
                istringstream iss(line.substr(2));
                Face face;
                string vertexInfo;
                while (getline(iss, vertexInfo, ' ')) {
                    istringstream vertexIss(vertexInfo);
                    VertexIndex vertexIndex;
                    vertexIss >> vertexIndex.v;
                    if (vertexIss.peek() == '/') {
                        vertexIss.ignore(); 
                        if (vertexIss.peek() != '/') {
                            vertexIss >> vertexIndex.vt;
                        }
                        else {
                            TextureMod = true;
                        }

                        if (vertexIss.peek() == '/') {
                            vertexIss.ignore(); 
                            vertexIss >> vertexIndex.vn;
                        }
                    }
                    face.facevertex.push_back(vertexIndex);
                }

                mFaces.push_back(face);

                // Compute face normal for new face
                computeFaceNormals(mFaces.back());
                
                // add edge
                mEdges.push_back({ face.facevertex[0].v, face.facevertex[1].v });
                mEdges.push_back({ face.facevertex[1].v, face.facevertex[2].v });
                mEdges.push_back({ face.facevertex[2].v, face.facevertex[0].v });

                // Add face index to adjacentFaces of each vertex
                for (const auto& vertexIndex : face.facevertex) {
                    mVertices[vertexIndex.v - 1].adjacentFaces.push_back(mFaces.size() - 1);
                }

            }

        }
        file.close();
        // for 5.2 mod 2
        if(TextureMod){
            setAllVertexUV();
            cout << "TextureMod : 2" << endl;
        }

        //add for computeVertexNormals
        for (auto& vertex : mVertices) {
            computeVertexNormals(vertex);
        }

        // Add texture coordinates to vertices
        for (const auto& face : mFaces){
            for (const auto& vertexIndex : face.facevertex) {
                Vertex& vertex = mVertices[vertexIndex.v - 1];
                if (vertexIndex.vt != 0) {
                    vertex.u = mTexCoords[vertexIndex.vt - 1].u;
                    vertex.v = mTexCoords[vertexIndex.vt - 1].v;
                }
            }
        }
        
    }
    else {
        cout << "Error: " << filename << endl;
    }

}

void Obj::drawModelPoints() const {
    glPointSize(1.0);
    for (const auto& vertex : mVertices) {
        glBegin(GL_POINTS);
        glVertex3f(vertex.x, vertex.y, vertex.z);
        glEnd();
    }
}

void Obj::drawModelLines() const {
    for (const auto& edge : mEdges) {
        const Vertex& v1 = mVertices[edge.v1 - 1];
        const Vertex& v2 = mVertices[edge.v2 - 1];

        glBegin(GL_LINES);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glEnd();
    }
}

void Obj::drawModelSolid() const {
    //texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    for (const auto& face : mFaces) {
        glBegin(GL_TRIANGLES);
        for (int i = 1; i < face.facevertex.size() - 1; ++i) {
            const Vertex& v1 = mVertices[face.facevertex[0].v - 1];
            const Vertex& v2 = mVertices[face.facevertex[i].v - 1];
            const Vertex& v3 = mVertices[face.facevertex[i + 1].v - 1];

            glNormal3f(v1.normal.x, v1.normal.y, v1.normal.z);
            glTexCoord2f(v1.u, v1.v);
            glVertex3f(v1.x, v1.y, v1.z);
            glNormal3f(v2.normal.x, v2.normal.y, v2.normal.z);
            glTexCoord2f(v2.u, v2.v);
            glVertex3f(v2.x, v2.y, v2.z);
            glNormal3f(v3.normal.x, v3.normal.y, v3.normal.z);
            glTexCoord2f(v3.u, v3.v);
            glVertex3f(v3.x, v3.y, v3.z);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void Obj::drawModelSolidFaceNormal() const {
    //texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    for (const auto& face : mFaces) {
        glBegin(GL_TRIANGLES);
        glNormal3f(face.normal.x, face.normal.y, face.normal.z);
        // for polygon > 3
        for (int i = 1; i < face.facevertex.size() - 1; ++i) {
            const Vertex& v1 = mVertices[face.facevertex[0].v - 1];
            const Vertex& v2 = mVertices[face.facevertex[i].v - 1];
            const Vertex& v3 = mVertices[face.facevertex[i + 1].v - 1];

            glTexCoord2f(v1.u, v1.v);
            glVertex3f(v1.x, v1.y, v1.z);
            glTexCoord2f(v2.u, v2.v);
            glVertex3f(v2.x, v2.y, v2.z);
            glTexCoord2f(v3.u, v3.v);
            glVertex3f(v3.x, v3.y, v3.z);

        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void Obj::computeFaceNormals(Face& face) {
    const Vertex& v1 = mVertices[face.facevertex[0].v - 1];
    const Vertex& v2 = mVertices[face.facevertex[1].v - 1];
    const Vertex& v3 = mVertices[face.facevertex[2].v - 1];

    // Calculate two edge vectors of the face
    Vector3D edge1 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
    Vector3D edge2 = { v3.x - v1.x, v3.y - v1.y, v3.z - v1.z };

    // Calculate the face normal using cross product of the edge vectors
    Vector3D normal = {
        edge1.y * edge2.z - edge1.z * edge2.y,
        edge1.z * edge2.x - edge1.x * edge2.z,
        edge1.x * edge2.y - edge1.y * edge2.x
    };

    // Normalize the normal vector
    float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    face.normal = { normal.x / length, normal.y / length, normal.z / length };
}

void Obj::computeVertexNormals(Vertex& vertex){

    Vector3D vertexNormalSum = { 0.0f, 0.0f, 0.0f };

    // faceIndex is elenment of adjacentFaces
    for (int faceIndex : vertex.adjacentFaces) {
        // faceIndex is 1 ~ n
        const Face& face = mFaces[faceIndex];
        vertexNormalSum.x += face.normal.x;
        vertexNormalSum.y += face.normal.y;
        vertexNormalSum.z += face.normal.z;
    }

    float length = sqrt(vertexNormalSum.x * vertexNormalSum.x 
                            + vertexNormalSum.y * vertexNormalSum.y 
                            + vertexNormalSum.z * vertexNormalSum.z);

    if (length != 0.0f) {
        vertexNormalSum.x /= length;
        vertexNormalSum.y /= length;
        vertexNormalSum.z /= length;
    }

    vertex.normal = vertexNormalSum;
    
};

void Obj::loadTexture(const char* filename){

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    // parameters for texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); 

    int width, height, numChannels;
    unsigned char* image = stbi_load(filename, &width, &height, &numChannels, 0);

    if (image) {
        // Flip the image vertically
        for (int y = 0; y < height / 2; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < numChannels; c++) {
                    unsigned char temp = image[(y * width + x) * numChannels + c];
                    image[(y * width + x) * numChannels + c] = 
                        image[((height - 1 - y) * width + x) * numChannels + c];
                    image[((height - 1 - y) * width + x) * numChannels + c] = temp;
                }
            }
        }

        GLenum format = 0;
        if (numChannels == 3){ 
            format = GL_RGB; 
            cout << "texture is GL_RGB" << endl;
        }
        else if (numChannels == 4) {
            format = GL_RGBA;
            cout << "texture is GL_RGBA" << endl;
        }
        else{
            cout << "Error: Unknown image format" << endl;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
        cout << "loading texture" << endl;

    } 
    else {
        cout << "Failed to load texture" << endl;
    }
};

void Obj::setAllVertexUV() {
    if (mVertices.size() != mTexCoords.size()) {
        cout << "vertices and texture does not match." << endl;
        return;
    }

    for (int i = 0; i < mVertices.size(); ++i) {
            mVertices[i].u = mTexCoords[i].u;
            mVertices[i].v = mTexCoords[i].v;
    }
};

void Obj::deformObject(float a, float t) {
    for (int i = 0; i < mVertices.size(); ++i) {
        Vertex& vertex = mVertices[i];
        vertex.x += 0.005 * sin(a + t);
        vertex.y += 0.005 * sin(a + t);
        vertex.z += 0.005 * sin(a + t);
    }
}