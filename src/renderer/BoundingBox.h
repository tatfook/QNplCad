#pragma once
#include <QVector3D>
class BoundingBox
{
public:

	QVector3D m_min;
	QVector3D m_max;

	inline QVector3D getCenter() const { return 0.5f*(m_min + m_max); }
	void extend(const QVector3D& vec);
};