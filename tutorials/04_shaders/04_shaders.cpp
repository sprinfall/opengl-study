#include <cstdio>
#include <string>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_util.h"
#include "ogldev_math_3d.h"

GLuint g_vbo;

const char* kVSFileName = "shader.vs";  // Vertex shader
const char* kFSFileName = "shader.fs";  // Fragment shader

static const char* DescribeError(GLenum gl_error) {
  switch (gl_error) {
    case GL_INVALID_ENUM:
      return "Invalid enum";
    case GL_INVALID_VALUE:
      return "Invalid value";
    case GL_INVALID_OPERATION:
      return "Invalid operation";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "Invalid framebuffer operation";
    case GL_OUT_OF_MEMORY:
      return "Out of memory";
    default:
      return "Unknown error";
  }
}

static void CheckError() {
  GLenum gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    printf("GL error: %s\n", DescribeError(gl_error));
  }
}

static void RenderSceneCB() {
  glClear(GL_COLOR_BUFFER_BIT);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(0);

  glutSwapBuffers();
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
    exit(0);
  }

  // 在 编译 shader 之前，指定它的 source。
  // Shader 的 source 可以分布于多个字符数组。
  const GLchar* texts[1] = { shader_text };
  GLint lengths[1] = { (GLint)std::strlen(shader_text) };
  // 原本是 C 风格的写法：
  // GLint lengths[1];
  // lengths[0] = strlen(shader_text)  // 注意：并没有 size_t -> int 的警告！
  glShaderSource(shader_object, 1, texts, lengths);

  glCompileShader(shader_object);

  // 获取编译状态，显示编译错误。
  // glGetShaderiv 中的 'iv' 代表 vector of int，详见：
  //   https://stackoverflow.com/a/15440261
  GLint status = GL_FALSE;
  glGetShaderiv(shader_object, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLchar info_log[1024];
    glGetShaderInfoLog(shader_object, 1024, NULL, info_log);
    fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader_type,
            info_log);
    exit(1);
  }

  // 把编译好的 shader 对象添加到程序对象。
  // 这个跟在 makefile 里指定一列链接对象非常类似。
  glAttachShader(shader_program, shader_object);
}

static void CompileShaders() {
  // Set up the shaders by creating a program object.
  // We will link all the shaders together into this object.
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
  if (status == 0) {
    glGetProgramInfoLog(shader_program, sizeof(error_log), NULL, error_log);
    fprintf(stderr, "Error linking shader program: '%s'\n", error_log);
    exit(1);
  }

  // 即使成功链接了，也要验证。链接基于 shaders 的组合检查有没有错误，而验证基于
  // 当前管道(pipeline)状态检查程序能否运行。在复杂程序中，最好每次调用draw之前
  // 都能验证。但是，此验证一般只应在开发过程中使用，最终产品应避免以节省开销。
  glValidateProgram(shader_program);

  glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &status);
  if (!status) {
    glGetProgramInfoLog(shader_program, sizeof(error_log), NULL, error_log);
    fprintf(stderr, "Invalid shader program: '%s'\n", error_log);
    exit(1);
  }

  // 把链接好的 shader 程序放到管道里。此程序对所有的 draw 调用都会一直保持有效
  // 直到被替换或禁用 (glUseProgram(NULL))。
  glUseProgram(shader_program);
}

int main(int argc, char** argv) {
  // Request a core profile context, otherwise we will be stuck to GL-3.0.
  // See: https://stackoverflow.com/a/40573748
  glutInitContextVersion(4, 5);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  // glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);  // TODO: Why not RGBA?

  glutInitWindowSize(1024, 768);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Tutorial 04 - Shaders");

  glutDisplayFunc(RenderSceneCB);

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

  CreateVertexBuffer();

  CompileShaders();

  if (glGetError() != GL_NO_ERROR) {
    printf("GL error\n");
  }

  glutMainLoop();

  return 0;
}
