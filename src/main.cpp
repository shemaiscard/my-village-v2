#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include "Shader.h"
#include "Scene.h"
#include "MathUtils.h"

Shader* shader;
Scene*  scene;
const int W = 1100, H = 700;

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Set projection for shader
    Math::Mat4 proj = Math::Mat4::Ortho(0, W, 0, H, -1, 1);
    shader->use();
    shader->setMat4("projection", &proj.m[0]);

    scene->render(*shader);

    glutSwapBuffers();
}

void timer(int)
{
    scene->update(0.016f);
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int, int)
{
    if (key == 'r' || key == 'R') scene->toggleRain();
    if (key == 27) exit(0); // ESC
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutCreateWindow("My Village V2");

    // GLEW / context is ready — create objects now
    scene  = new Scene();
    shader = new Shader("assets/shaders/scene.vert",
                        "assets/shaders/scene.frag");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}
