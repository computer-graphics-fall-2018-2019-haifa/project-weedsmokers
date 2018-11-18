#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera()
{
	viewTransformation = glm::mat4(1.0);
	projectionTransformation = glm::mat4(1.0);
}

Camera::Camera(const glm::vec4& eye , const glm::vec4& at , const glm::vec4& up ) :
	zoom(1.0)
{
	viewTransformation = glm::mat4(1.0);
	projectionTransformation = glm::mat4(1.0);
	SetCameraLookAt(eye, at, up);
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	glm::vec3 z = normalize(eye-at);
	glm::vec3 x = normalize(cross(up,z));
	glm::vec3 y = normalize(cross(z, x));
	glm::vec4 t = glm::vec4(0.0, 0.0, 0.0, 1.0);

	projectionTransformation[0][0] = x.x;
	projectionTransformation[0][1] = x.y;
	projectionTransformation[0][2] = x.z;
	projectionTransformation[1][0] = y.x;
	projectionTransformation[1][1] = y.y;
	projectionTransformation[1][2] = y.z;
	projectionTransformation[2][0] = z.x;
	projectionTransformation[2][1] = z.y;
	projectionTransformation[2][2] = z.z;
	projectionTransformation[3][0] = eye.x;
	projectionTransformation[3][1] = eye.y;
	projectionTransformation[3][2] = eye.z;
	translate(projectionTransformation,eye); // not sure
	


}

void Camera::SetOrthographicProjection(
	const float height,
	const float aspectRatio,
	const float near,
	const float far)
{

}

void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float near,
	const float far)
{

}

void Camera::SetZoom(const float zoom)
{





}