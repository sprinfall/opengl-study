#include <cmath>
#include <cstdio>
#include <string>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"
#include "utility.h"

GLuint g_vbo;
GLuint g_world_location;

static void RenderSceneCB() {
  glClear(GL_COLOR_BUFFER_BIT);

  static float scale = 0.0f;

  scale += 0.001f;

  // Matrix4f.m : float m[4][4]
  Matrix4f world;

  world.InitTranslationTransform(sinf(scale), 0.0f, 0.0f);
  // 或者像下面这样：
  // world.InitIdentity();
  // world.m[0][3] = sinf(scale);
  // 原来的写法为（太复杂了）：
  // world.m[0][0] = 1.0f; world.m[0][1] = 0.0f; world.m[0][2] = 0.0f; world.m[0][3] = sinf(scale);
  // world.m[1][0] = 0.0f; world.m[1][1] = 1.0f; world.m[1][2] = 0.0f; world.m[1][3] = 0.0f;
  // world.m[2][0] = 0.0f; world.m[2][1] = 0.0f; world.m[2][2] = 1.0f; world.m[2][3] = 0.0f;
  // world.m[3][0] = 0.0f; world.m[3][1] = 0.0f; world.m[3][2] = 0.0f; world.m[3][3] = 1.0f;

  // 把 matrix 传给 shader。
  // 注意最后一个参数不能直接写成 world.m，因为它的类型是 float (*)[4]。
  glUniformMatrix4fv(g_world_location, 1, GL_TRUE, &world.m[0][0]);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(0);

  glutSwapBuffers();
}

static void InitializeGlutCallbacks() {
  glutDisplayFunc(RenderSceneCB);
  glutIdleFunc(RenderSceneCB);
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
  glutInitContextVersion(4, 5);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  glutInitWindowSize(1024, 768);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("06 - Translation Transform");

  InitializeGlutCallbacks();

  // Must be done after glut is initialized!
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    return 1;
  }

  printf("GL version: %s\n", glGetString(GL_VERSION));

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // 关于 VAO，详见 04_shaders 示例里的注释。
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  CreateVertexBuffer();

  GLuint shader_program = CreateProgram("shader.vs", "shader.fs");

  // 拿到一致变量的位置（且只有在链接之后才能拿到这个位置），随后便可通过
  // glUniform 设置一致变量的值，或通过 glGetUniform 得到它的值。
  g_world_location = glGetUniformLocation(shader_program, "gWorld");
  assert(g_world_location != 0xFFFFFFFF);

  glutMainLoop();

  return 0;
}
