#include "Camera.h"
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QtMath>
#include "BoundingBox.h"
using namespace QNplCad;

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
}

double Camera::zoomValue()
{
	return viewer_distance;
}

void Camera::viewAll(BoundingBox &bbox)
{
	QVector3D d = bbox.diagonal();
	double bboxRadius = d.lengthSquared() / 2;
	double radius = (bbox.getCenter() - QVector3D(0.0f, 0.0f, 0.0f)).lengthSquared() + bboxRadius;
	double distance = radius / sin(this->fov / 2 * M_PI / 180);
	this->viewer_distance = distance;
}



