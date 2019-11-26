#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera():eye(glm::vec4(0, 0, 1.0, 0)),at(glm::vec4(0, 0, 0, 0)),up(glm::vec4(0, 1.0, 0, 0))
{
	viewTransformation = glm::mat4(1.0);
	projectionTransformation = glm::mat4(1.0);
	eye = glm::vec4(0, 0, 1.0, 0);
	at = (glm::vec4(0, 0, 0, 0));
	up = glm::vec4(0, 1.0, 0, 0);
	
}

Camera::Camera(const glm::vec4& eye , const glm::vec4& at , const glm::vec4& up ) :	zoom(1.0),  eye(glm::vec4(eye)), at(glm::vec4(at)), up(glm::vec4(up))
{
	viewTransformation = glm::mat4(1.0);
	projectionTransformation = glm::mat4(1.0);
	SetCameraLookAt(eye, at, up);
	this->eye = eye;
	this->at = at;
	this->up = up;
}

Camera::~Camera()
{
}

Camera & Camera::operator=(const Camera & other)
{
	//Camera a= Camera(other.getEye(), other.getAt(), other.getUp());
	this->eye = glm::vec4(other.getEye());
	this->up = glm::vec4(other.getUp());
	this->at = glm::vec4(other.getAt());
	this->viewTransformation = other.getviewTransformation();
	this->projectionTransformation = other.getProjectionTransformation();
	return *this;
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{


	this->eye = glm::vec4(eye,0);
	this->at = glm::vec4( at,0 );
	this->up = glm::vec4(up, 0);
	/*glm::vec3 z = normalize(eye-at);
	glm::vec3 x = normalize(cross(up,z));
	glm::vec3 y = normalize(cross(z, x));
	glm::vec4 t = glm::vec4(0.0, 0.0, 0.0, 1.0);

	viewTransformation[0][0] = x.x;
	viewTransformation[0][1] = x.y;
	viewTransformation[0][2] = x.z;
	viewTransformation[1][0] = y.x;
	viewTransformation[1][1] = y.y;
	viewTransformation[1][2] = y.z;
	viewTransformation[2][0] = z.x;
	viewTransformation[2][1] = z.y;
	viewTransformation[2][2] = z.z;
	viewTransformation[3][0] = eye.x;
	viewTransformation[3][1] = eye.y;
	viewTransformation[3][2] = eye.z;
	translate(viewTransformation,eye); // not sure*/
	/*glm::vec3 dir = glm::normalize(eye - at);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, dir));
	glm::vec3 cameraUp = glm::cross(dir,cameraRight);
	glm::mat4 M1 = glm::mat4(0);
	glm::mat4 M2 = glm::mat4(0);
	M1[0][1] = cameraRight.x;
	M1[0][2] = cameraRight.y;
	M1[0][3] = cameraRight.z;
	M1[1][1] = up.x;
	M1[1][2] = up.y;
	M1[1][3] = up.z;
	M1[2][1] = dir.x;
	M1[2][2] = dir.y;
	M1[2][3] = dir.z;
	M1[3][3] = 1;

	M2[0][0] = 1;
	M2[1][1] = 1;
	M2[2][2] = 1;
	M2[3][3] = 1;
	M2[0][3] = -eye.x;
	M2[1][3] = -eye.y;
	M2[2][3] = -eye.z;
	viewTransformation = M1 * M2;
	M2 = glm::lookAt(eye, at, up);*/
	glm::vec3 f, s, u;
	f = normalize(at - eye);
	s=normalize(cross(f, up));
	u = cross(s, f);
	viewTransformation[0][0] = s.x;
	viewTransformation[1][0] = s.y;
	viewTransformation[2][0] = s.z;
	viewTransformation[0][1] = u.x;
	viewTransformation[1][1] = u.y;
	viewTransformation[2][1] = u.z;
	viewTransformation[0][2] = f.x;
	viewTransformation[1][2] = f.y;
	viewTransformation[2][2] = f.z;
	viewTransformation[3][0] = -dot(s, eye);
	viewTransformation[3][1] = -dot(u, eye);
	viewTransformation[3][2] = -dot(f, eye);







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
	projectionTransformation=glm::perspective(fovy, aspectRatio, near, far);

}

void Camera::SetZoom(const float zoom)
{
	this->zoom = zoom;




}

float Camera::getZoom() const
{
	return zoom;
}

glm::mat4x4 Camera::getviewTransformation() const
{
	return glm::mat4x4(viewTransformation);
}

glm::mat4x4 Camera::getProjectionTransformation() const
{
	glm::mat4x4 A= projectionTransformation;
	return A;
}

void Camera::setCameraorthographic(float left, float right, float bottom, float top,float Near,float Far)
{
	projectionTransformation = glm::ortho( left,  right,  bottom,  top,Near,Far);


}

glm::vec4 Camera::getEye() const
{
	return  (glm::vec4(eye));
}

glm::vec4 Camera::getUp() const
{
	return  (glm::vec4(up));
}

glm::vec4 Camera::getAt() const
{
	return  (glm::vec4(at));;
}

