#ifndef UTILITY_H_
#define UTILITY_H_

// TODO: Try to avoid include glew.h in a header.
#include <GL/glew.h>

// Check GL error, print it if any.
void CheckError();

GLuint LoadShader(const char* shader_filename, GLenum shader_type);

GLuint CreateProgram(const char* vert_shader_path,
                     const char* frag_shader_path);

#endif  // UTILITY_H_
