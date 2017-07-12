#pragma once
#include <QVector3D>
namespace QNplCad
{
	class BoundingBox
	{
	public:
		BoundingBox();
		BoundingBox(QVector3D min_, QVector3D max_);
		QVector3D m_min;
		QVector3D m_max;

		inline QVector3D getCenter() const { return 0.5f*(m_min + m_max); }
		float		getWidth()								const { return m_max.x() - m_min.x(); }
		float		getHeight()								const { return m_max.y() - m_min.y(); }
		float		getDepth()								const { return m_max.z() - m_min.z(); }
		void extend(const QVector3D& vec);
		QVector3D diagonal();
	};
}
