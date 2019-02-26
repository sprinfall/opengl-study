#include "utility.h"

#include <cstdio>
#include <string>

#include "ogldev_util.h"

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

void CheckError() {
  GLenum gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    printf("GL error: %s\n", DescribeError(gl_error));
  }
}

GLuint LoadShader(const char* shader_filename, GLenum shader_type) {
  std::string shader_text;
  if (!ReadFile(shader_filename, shader_text)) {
    exit(1);
  }

  GLuint shader = glCreateShader(shader_type);
  if (shader == 0) {
    fprintf(stderr, "Error creating shader type %d\n", shader_type);
    exit(0);
  }

  // 在 编译 shader 之前，指定它的 source。
  // Shader 的 source 可以分布于多个字符数组。
  const GLchar* texts[1] = {shader_text.c_str()};
  GLint lengths[1] = {(GLint)shader_text.size()};
  glShaderSource(shader, 1, texts, lengths);

  glCompileShader(shader);

  // 检查编译状态，显示编译错误。
  // glGetShaderiv 中的 'iv' 代表 vector of int，详见：
  //   https://stackoverflow.com/a/15440261
  GLint status = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLchar info_log[1024];
    glGetShaderInfoLog(shader, 1024, NULL, info_log);
    fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader_type,
            info_log);
    exit(1);
  }

  return shader;
}

GLuint CreateProgram(const char* vert_shader_path,
                     const char* frag_shader_path) {
  // Load and compile the vertex and fragment shaders.
  GLuint vert_shader = LoadShader(vert_shader_path, GL_VERTEX_SHADER);
  GLuint frag_shader = LoadShader(frag_shader_path, GL_FRAGMENT_SHADER);

  // Set up the shaders by creating a program object.
  // We will link all the shaders together into this object.
  GLuint shader_program = glCreateProgram();
  if (shader_program == 0) {
    fprintf(stderr, "Error creating shader program\n");
    exit(1);
  }

  // 把编译好的 shader 对象添加到程序对象。
  // 这个跟在 makefile 里指定一列链接对象非常类似。
  glAttachShader(shader_program, vert_shader);
  glAttachShader(shader_program, frag_shader);

  // Flag the shaders for deletion.
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  glLinkProgram(shader_program);

  GLint status = 0;
  GLchar error_log[1024] = {0};
  glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
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

  return shader_program;
}
