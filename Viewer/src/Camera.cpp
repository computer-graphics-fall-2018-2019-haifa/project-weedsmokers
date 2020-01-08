#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


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

	viewTransformation = glm::lookAt(eye, at, up);





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

