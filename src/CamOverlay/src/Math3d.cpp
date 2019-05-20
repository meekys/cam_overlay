#define MATH_3D_IMPLEMENTATION

#include <CamOverlay/Math3d.hpp>

mat4_t m4_perspective2(float vertical_field_of_view_in_deg, float aspect_ratio, float near, float far) {
    float top = near * tan(M_PI / 180 * vertical_field_of_view_in_deg / 2.0f);
    float bottom = -top;
    float right = top * aspect_ratio;
    float left = -right;

	return mat4(
		(2.0f * near) / (right - left), 0,                              (right + left) / (right - left), 0,
		0,                              (2.0f * near) / (top - bottom), (top + bottom) / (top - bottom), 0,
		0,                              0,                             -((far + near) / (far - near)),  -((2.0f * far * near) / (far - near)),
		0,                              0,                              1,                               0
	);
}