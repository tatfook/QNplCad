#include "Camera.h"
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QMatrix3x3>

Camera::Camera(QVector3D eye, QVector3D lookat, QVector3D up, float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
	: m_type(Camera::Type::PERSPECTIVE)
	, m_eye(eye)
	, m_lookat(lookat)
	, m_up(up)
	, m_pitch(0.0f)
	, m_yaw(0.0f)
	, m_roll(0.0f)
	, m_fieldOfView(fieldOfView)
	, m_aspectRatio(aspectRatio)
	, m_nearPlane(nearPlane)
	, m_farPlane(farPlane)
	, m_viewer_distance(1)
{

}

float Camera::getFieldOfView() const
{
	return m_fieldOfView;
}

void Camera::setFieldOfView(float v)
{
	m_fieldOfView = v;
}

float Camera::getAspectRatio() const
{
	return m_aspectRatio;
}

void Camera::setAspectRatio(float v)
{
	m_aspectRatio = v;
}

float Camera::getNearPlane() const
{
	return m_nearPlane;
}

void Camera::setNearPlane(float v)
{
	m_nearPlane = v;
}

float Camera::getFarPlane() const
{
	return m_farPlane;
}

void Camera::setFarPlane(float v)
{
	m_farPlane = v;
}

void Camera::zoom(int delta)
{
	m_viewer_distance *= pow(0.9, delta / 120.0);
	printf("m_viewer_distance:%f\n", m_viewer_distance);
}

double Camera::zoomValue()
{
	return m_viewer_distance;
}

void Camera::rotate(float pitch, float yaw, float roll)
{
	m_pitch = pitch;
	m_yaw = yaw;
	m_roll = roll;
}

void Camera::translate(const QVector3D &vLocal)
{
	m_translation = vLocal;
}

QMatrix4x4 Camera::projectionMatrix()
{
	QMatrix4x4 m;
	m.perspective(m_fieldOfView, m_aspectRatio, m_viewer_distance * m_nearPlane, m_viewer_distance * m_farPlane);
	return m;
}

QMatrix4x4 Camera::viewMatrix()
{
	//Understanding the View Matrix:https://www.3dgep.com/understanding-the-view-matrix/#The_View_Matrix
	QMatrix4x4 m;

	QMatrix4x4 rot_x;		rot_x.rotate(m_pitch, QVector3D(1, 0, 0));
	QMatrix4x4 rot_y;		rot_y.rotate(m_yaw, QVector3D(0, 1, 0));
	QMatrix4x4 rot_z;		rot_z.rotate(m_roll, QVector3D(0, 0, 1));
	QMatrix4x4 camera_rot = rot_x * rot_y * rot_z;
	QVector3D world_trans = camera_rot * m_translation;
	QVector3D world_up = camera_rot * m_up;
	QVector3D world_eye = m_eye + world_trans;
	QVector3D world_lookat = camera_rot * m_lookat + world_eye;

	return LookAtRH(world_eye,world_lookat,world_up);
}

QMatrix4x4 Camera::LookAtRH(const QVector3D& eye, const QVector3D& target, const QVector3D& up)
{
	QVector3D zaxis = (eye - target).normalized();    // The "forward" vector.
	QVector3D xaxis = (QVector3D::crossProduct(up, zaxis)).normalized();// The "right" vector.
	QVector3D yaxis = QVector3D::crossProduct(zaxis, xaxis);     // The "up" vector.

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
	QMatrix4x4 viewMatrix(
		xaxis.x(),            yaxis.x(),            zaxis.x(),       0,
		xaxis.y(),            yaxis.y(),            zaxis.y(),       0,
		xaxis.z(),            yaxis.z(),            zaxis.z(),       0,
		-QVector3D::dotProduct(xaxis, eye), -QVector3D::dotProduct(yaxis, eye), -QVector3D::dotProduct(zaxis, eye),  1
	);

	return viewMatrix;
}
