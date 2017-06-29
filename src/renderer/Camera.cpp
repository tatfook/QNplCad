#include "Camera.h"
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include "BoundingBox.h"
Camera::Camera()
	: fov(22.5)
	, object_trans(QVector3D(0, 0, 0))
	, object_rot(QVector3D(35, 0, 25))
	, viewer_distance(500)
{

}
void Camera::zoom(int delta)
{
	viewer_distance *= pow(0.9, delta / 120.0);
	printf("viewer_distance:%f\n", viewer_distance);
}

double Camera::zoomValue()
{
	return viewer_distance;
}

void Camera::viewAll(const BoundingBox &bbox)
{

}



