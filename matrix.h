#ifndef MATRIX_H
#define MATRIX_H

#include "GLES/gl.h"

typedef GLfloat Matrix4[4][4];

extern const Matrix4 identitiyMatrix;

extern void ortho(Matrix4 matrix, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);
extern void perspective(Matrix4 matrix, GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far);

#endif