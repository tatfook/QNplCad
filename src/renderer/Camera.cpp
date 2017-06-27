#include "Camera.h"
#include <QMatrix4x4>
#include <QMatrix3x3>

Camera::Camera(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
	: m_type(Camera::Type::PERSPECTIVE)
	, m_eye(QVector3D(0.0f, 0.0f, 0.0f))
	, m_up(QVector3D(0.0f, 0.0f, 1.0f))
	, m_lookat(QVector3D(0.0f, 1.0f, 0.0f))
	, m_pitch(0.0f)
	, m_yaw(0.0f)
	, m_roll(0.0f)
	, m_fieldOfView(fieldOfView)
	, m_aspectRatio(aspectRatio)
	, m_nearPlane(nearPlane)
	, m_farPlane(farPlane)
	, m_viewer_distance(140)
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

QMatrix4x4 Camera::projectionMatrix() const
{
	QMatrix4x4 m;
	m.perspective(m_fieldOfView, m_aspectRatio, m_viewer_distance * m_nearPlane, m_viewer_distance * m_farPlane);
	return m;
}

QMatrix4x4 Camera::viewMatrix() const
{
	QMatrix4x4 m;

	QMatrix4x4 rot_x;		rot_x.rotate(m_pitch, QVector3D(1, 0, 0));
	QMatrix4x4 rot_y;		rot_y.rotate(m_yaw, QVector3D(0, 1, 0));
	QMatrix4x4 rot_z;		rot_z.rotate(m_roll, QVector3D(0, 0, 1));
	QMatrix4x4 camera_rot = rot_x * rot_y * rot_z;
	QVector3D world_up = camera_rot * m_up;
	QVector3D world_lookat = camera_rot * m_lookat;
	QVector3D world_eye = camera_rot * m_eye;
	return m;
}
