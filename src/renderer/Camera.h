#pragma once
#include <QMatrix4x4>
#include <QVector3D>
#include <QQuaternion>
 class Camera
 {
 public:
	 enum Type
	 {
		 PERSPECTIVE = 1,
		 ORTHOGRAPHIC = 2
	 };
	Camera(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

	float getFieldOfView() const;
	void setFieldOfView(float v);
	float getAspectRatio() const;
	void setAspectRatio(float v);
	float getNearPlane() const;
	void setNearPlane(float v);
	float getFarPlane() const;
	void setFarPlane(float v);
	void zoom(int delta);
	double zoomValue();

	void rotate(float pitch, float yaw, float roll);
	void translate(const QVector3D &vLocal);
	QMatrix4x4 projectionMatrix() const;
	QMatrix4x4 viewMatrix() const;
 private:
	 Camera::Type m_type;
	 QVector3D m_eye;
	 QVector3D m_up;
	 QVector3D m_lookat;
	 float m_pitch;
	 float m_yaw;
	 float m_roll;
	 QVector3D m_translation;
	 float m_fieldOfView;
	 float m_aspectRatio;
	 float m_nearPlane;
	 float m_farPlane;
 	// Perspective settings
 	double m_viewer_distance;
 };
