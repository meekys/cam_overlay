#ifndef DISPLAY_H
#define DISPLAY_H

#define check() assert(glGetError() == 0)

extern GLint compile_shader_program(char* vertex_source, char* fragment_source);

#endif