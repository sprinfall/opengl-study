#include <cstdio>
#include <string>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"
#include "utility.h"

GLuint g_vbo;

// A shader is a, typically small, program that is executed on the video card. A
// shader is written in GLSL, the OpenGL Shading Language, a language similar
// to C.

static void RenderSceneCB() {
  glClear(GL_COLOR_BUFFER_BIT);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(0);

  glutSwapBuffers();
}

static void InitializeGlutCallbacks() {
  glutDisplayFunc(RenderSceneCB);
}

static void CreateVertexBuffer() {
  Vector3f vertices[3];
  vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
  vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
  vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

  glGenBuffers(1, &g_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

int main(int argc, char** argv) {
  // 请求 core profile context，否则缺省只能使用 GL-3.0。
  // 详见：https://stackoverflow.com/a/40573748
  glutInitContextVersion(4, 5);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  // glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  glutInitWindowSize(1024, 768);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("04 - Shaders");

  InitializeGlutCallbacks();

  // Must be done after glut is initialized!
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    return 1;
  }

  printf("GL version: %s\n", glGetString(GL_VERSION));

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // 关于 VAO:
  // Ogldev 的例子并没有 VAO，也就是下面三行代码，结果就是窗口什么都不会显示（
  // 没有预期的红色三角）。
  // 这个问题困扰了我好几天，在网上搜了好久，今天终于在 StackOverflow 找到答案！
  // 怎么找到问题的呢？我先用 glGetError() 定位出错的函数，发现是 RenderSceneCB
  // 中的 glVertexAttribPointer，然后以 glVertexAttribPointer 和
  // GL_INVALID_OPERATION 为关键字搜索，发现了如下问题：
  //   https://stackoverflow.com/a/13405205
  // 简单来说，在 Core Profile 下，必须要有 VAO。
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  // 注意 VBO 用的是 glGenBuffers, glBindBuffer

  CreateVertexBuffer();

  // 关于文件名：vert.shader 和 frag.shader 也不错
  CreateProgram("shader.vs", "shader.fs");

  glutMainLoop();

  return 0;
}
