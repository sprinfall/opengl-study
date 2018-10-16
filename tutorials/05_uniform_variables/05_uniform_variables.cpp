#include <cstdio>
#include <string>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"
#include "ogldev_util.h"

GLuint g_vbo;
GLuint g_scale_location;

const char* kVSFileName = "shader.vs";
const char* kFSFileName = "shader.fs";

static void RenderSceneCB() {
  glClear(GL_COLOR_BUFFER_BIT);

  static float scale = 0.0f;

  scale += 0.001f;

  // 设置新的值到一致变量。
  // 注意，此例没有通过 glGetUniform 去拿一致变量的值，应该这个值我们存在了静态
  // 变量中。
  glUniform1f(g_scale_location, sinf(scale));

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

static void AddShader(GLuint shader_program, const char* shader_text,
                      GLenum shader_type) {
  GLuint shader_object = glCreateShader(shader_type);

  if (shader_object == 0) {
    fprintf(stderr, "Error creating shader type %d\n", shader_type);
    exit(1);
  }

  const GLchar* texts[1] = {shader_text};
  GLint lengths[1] = {(GLint)std::strlen(shader_text)};
  glShaderSource(shader_object, 1, texts, lengths);

  glCompileShader(shader_object);

  GLint status = GL_FALSE;
  glGetShaderiv(shader_object, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLchar info_log[1024];
    glGetShaderInfoLog(shader_object, 1024, NULL, info_log);
    fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader_type,
            info_log);
    exit(1);
  }

  glAttachShader(shader_program, shader_object);
}

static void CompileShaders() {
  GLuint shader_program = glCreateProgram();

  if (shader_program == 0) {
    fprintf(stderr, "Error creating shader program\n");
    exit(1);
  }

  std::string vs;
  std::string fs;

  if (!ReadFile(kVSFileName, vs)) {
    exit(1);
  }

  if (!ReadFile(kFSFileName, fs)) {
    exit(1);
  }

  AddShader(shader_program, vs.c_str(), GL_VERTEX_SHADER);
  AddShader(shader_program, fs.c_str(), GL_FRAGMENT_SHADER);

  GLint status = 0;
  GLchar error_log[1024] = {0};

  glLinkProgram(shader_program);

  glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    glGetProgramInfoLog(shader_program, sizeof(error_log), NULL, error_log);
    fprintf(stderr, "Error linking shader program: '%s'\n", error_log);
    exit(1);
  }

  glValidateProgram(shader_program);

  glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &status);
  if (status != GL_TRUE) {
    glGetProgramInfoLog(shader_program, sizeof(error_log), NULL, error_log);
    fprintf(stderr, "Invalid shader program: '%s'\n", error_log);
    exit(1);
  }

  glUseProgram(shader_program);

  // 拿到一致变量的位置（且只有在链接之后才能拿到这个位置），随后便可通过
  // glUniform 设置一致变量的值，或通过 glGetUniform 得到它的值。
  g_scale_location = glGetUniformLocation(shader_program, "gScale");
  assert(g_scale_location != 0xFFFFFFFF);
}

int main(int argc, char** argv) {
  glutInitContextVersion(4, 5);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  glutInitWindowSize(1024, 768);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Tutorial 05 - Uniform Variables");

  InitializeGlutCallbacks();

  // Must be done after glut is initialized!
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    return 1;
  }

  printf("GL version: %s\n", glGetString(GL_VERSION));

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  CreateVertexBuffer();

  CompileShaders();

  glutMainLoop();

  return 0;
}
