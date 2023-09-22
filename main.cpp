#include <GL/glut.h>
#include <iostream>
#include "Obj.h"
#include <cmath>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

Obj obj;

int displayMode = 0;
float angleX = 0.0;
float angleY = 0.0;
float angleZ = 0.0;

float scaleFactor = 1.0;

float lightDistance = 5.0; 
float lightAngle = 0.0;

int savedImageCount = 0;

void flipImageVertically(BYTE* pixels, int width, int height) {
    const int bytesPerPixel = 3;
    BYTE* tempRow = new BYTE[bytesPerPixel * width];

    for (int y = 0; y < height / 2; ++y) {
        //copy current row to tempRow
        memcpy(tempRow, &pixels[y * bytesPerPixel * width], 
                bytesPerPixel * width);
        //store opposite row to current row
        memcpy(&pixels[y * bytesPerPixel * width], 
                &pixels[(height - 1 - y) * bytesPerPixel * width], bytesPerPixel * width);
        //store tempRow to opposite row
        memcpy(&pixels[(height - 1 - y) * bytesPerPixel * width], 
                tempRow, bytesPerPixel * width);
    }

    delete[] tempRow;
}

void saveImage(const char* filename, int width, int height) {
    BYTE* pixels = new BYTE[3 * width * height];

    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    flipImageVertically(pixels, width, height);

    string fullFilename = string(filename) + to_string(savedImageCount) + ".png";
    savedImageCount++;

    if (stbi_write_png(fullFilename.c_str(), width, height, 3, pixels, width * 3) != 0) {
        std::cout << "save" << std::endl;
    } else {
        std::cout << "fail" << std::endl;
    }

    delete[] pixels;
}

void displayFunction() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 10.0, 
                0.0, 0.0, 0.0, 
                0.0, 1.0, 0.0);

    glTranslatef(0.0, 0.0, 0.0);
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
    glRotatef(angleZ, 0.0, 0.0, 1.0);
    glScalef(scaleFactor, scaleFactor, scaleFactor);

    glColor3f(1.0, 1.0, 1.0);

    if (displayMode == 0) {
        obj.drawModelPoints();
    }
    else if (displayMode == 1) {
        obj.drawModelLines();
    }
    else if (displayMode == 2) {
        obj.drawModelSolid();
    }
    else if (displayMode == 3) {
        obj.drawModelSolidFaceNormal();
    }
    glutSwapBuffers();
}

void reshapeFunction(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(width) / height, 0.1, 100.0);
}

void keyboardFunction(unsigned char key, int x, int y) {
    if (key == 's' || key == 'S') {
        displayMode = (displayMode + 1) % 4;
    }
    else if (key == 'z' || key == 'Z') {
        angleX += 10.0;
    }
    else if (key == 'x' || key == 'X') {
        angleY += 10.0;
    }
    else if (key == 'c' || key == 'C') {
        angleZ += 10.0;
    }
    else if (key == 'q' || key == 'Q') {
        scaleFactor += 0.1;
    }
    else if (key == 'e' || key == 'E') {
        scaleFactor -= 0.1;
        if (scaleFactor < 0.1) {
            scaleFactor = 0.1;
        }
    }
    else if (key == ' ') {
        angleX = 0.0;
        angleY = 0.0;
        scaleFactor = 1.0;
    }
    else if (key == 27) {
        exit(0);
    }
    else if (key == 'p' || key == 'P') {
        // add this line for saving image, don't need .png
        saveImage("output/output", 800, 600);
    }

    glutPostRedisplay();
}

void idleFunction() {
    const float a = 0.0;
    static float t = 0.0;
    t += 0.005;
    obj.deformObject(a, t);

    glutPostRedisplay();
}

void lighting(){
    glEnable(GL_LIGHTING);
    
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPosition[] = { 1.0f, 1.0f, 1.0f, 0.0f }; // 光源位置

    for (int i = 0; i < 4; ++i) {  // 啟用四個光源
        glEnable(GL_LIGHT0 + i);

        glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuseLight);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, specularLight);
        glLightfv(GL_LIGHT0 + i, GL_POSITION, lightPosition);
    }
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glEnable(GL_TEXTURE_2D);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OBJ Model Viewer");

    lighting();

    glEnable(GL_DEPTH_TEST);
    //obj.readFile("model/eye/eyeball.obj");
    obj.readFile("model/cap/capsule.obj");
    obj.loadTexture("model/cap/capsule0.jpg");

    glutDisplayFunc(displayFunction);
    glutReshapeFunc(reshapeFunction);
    glutKeyboardFunc(keyboardFunction);
    glutIdleFunc(idleFunction);

    glutMainLoop();

    return 0;
}

