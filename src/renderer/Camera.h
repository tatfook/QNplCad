#pragma once
#include <QMatrix4x4>
#include <QVector3D>
#include <QQuaternion>
class BoundingBox;
 class Camera
 {
 public:
	 Camera();
	void zoom(int delta);
	double zoomValue();

	double fov; // field of view
	QVector3D object_trans;
	QVector3D object_rot;
	void viewAll(BoundingBox &bbox);
 private:

 	double viewer_distance;
 };
