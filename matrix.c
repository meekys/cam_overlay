#include <math.h>

#include "matrix.h"

static const double pi = acos(-1.0);

// Derived from https://solarianprogrammer.com/2013/05/22/opengl-101-matrices-projection-view-model/

const Matrix4 identitiyMatrix =
{
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

extern void ortho(Matrix4 matrix, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far)
{
    matrix[0][0] = 2.0f / (right - left);
    matrix[0][1] = 0.0f;
    matrix[0][2] = 0.0f;
    matrix[0][3] = - ((right + left) / (right - left));

    matrix[1][0] = 0.0f;
    matrix[1][1] = 2.0f / (top - bottom);
    matrix[1][2] = 0.0f;
    matrix[1][3] = - ((top + bottom) / (top - bottom));

    matrix[2][0] = 0.0f;
    matrix[2][1] = 0.0f;
    matrix[2][2] = - (2.0f / (far - near));
    matrix[2][3] = - ((far + near) / (far - near));

    matrix[3][0] = 0.0f;
    matrix[3][1] = 0.0f;
    matrix[3][2] = 0.0f;
    matrix[3][3] = 1.0f;
}

extern void perspective(Matrix4 matrix, GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far)
{
    GLfloat top = near * tan(pi / 180 * fov / 2.0f);
    GLfloat bottom = -top;
    GLfloat right = top * aspect;
    GLfloat left = -right;

    matrix[0][0] = (2.0f * near) / (right - left);
    matrix[0][1] = 0.0f;
    matrix[0][2] = (right + left) / (right - left);
    matrix[0][3] = 0.0f;

    matrix[1][0] = 0.0f;
    matrix[1][1] = (2.0f * near) / (top - bottom);
    matrix[1][2] = (top + bottom) / (top - bottom);
    matrix[1][3] = 0.0f;

    matrix[2][0] = 0.0f;
    matrix[2][1] = 0.0f;
    matrix[2][2] = - ((far + near) / (far - near));
    matrix[2][3] = - ((2.0f * far * near) / (far - near));

    matrix[3][0] = 0.0f;
    matrix[3][1] = 0.0f;
    matrix[3][2] = -1.0f;
    matrix[3][3] = 0.0f;
}