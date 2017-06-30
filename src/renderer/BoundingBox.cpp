#include "BoundingBox.h"
#include <cmath>

BoundingBox::BoundingBox(QVector3D min_, QVector3D max_)
	: m_min(min_)
	, m_max(max_)
{

}

BoundingBox::BoundingBox()
	: m_min(QVector3D())
	, m_max(QVector3D())
{

}

void BoundingBox::extend(const QVector3D& vec)
{
	m_min[0] = fmin(m_min.x(), vec.x());
	m_min[1] = fmin(m_min.y(), vec.y());
	m_min[2] = fmin(m_min.z(), vec.z());
	m_max[0] = fmax(m_max.x(), vec.x());
	m_max[1] = fmax(m_max.y(), vec.y());
	m_max[2] = fmax(m_max.z(), vec.z());
}

QVector3D BoundingBox::diagonal()
{
	QVector3D d(getWidth(), getHeight(), getDepth());
	return d;
}

