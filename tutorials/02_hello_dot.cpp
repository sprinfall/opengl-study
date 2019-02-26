#include <cstdio>

#include <GL/glew.h>  // Must before GLUT!
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"

// A Vertex Buffer Object, or VBO, is a chunk of memory managed by OpenGL,
// basically it is a piece of the memory of your video card.
GLuint g_vbo;

static void RenderSceneCB() {
  glClear(GL_COLOR_BUFFER_BIT);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_POINTS, 0, 1);

  glDisableVertexAttribArray(0);

  glutSwapBuffers();
}

static void CreateVertexBuffer() {
  Vector3f vertices[1];
  vertices[0] = Vector3f(0.0f, 0.0f, 0.0f);

  // This function can create an array of handles if needed; for our particular
  // case we have a single VBO so one handle will suffice.
  glGenBuffers(1, &g_vbo);

  // Once a VBO is created, we need to bind it in order to modify or use it.
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);

  // Allocate space for the VBO and fill it with the content of our vertices
  // array.
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  glutInitWindowSize(1024, 768);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("02 - Hello Dot");

  glutDisplayFunc(RenderSceneCB);

  // Must be done after glut is initialized!
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    return 1;
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  CreateVertexBuffer();

  glutMainLoop();

  return 0;
}
